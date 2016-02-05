// -*- mode: c++ -*-


/*
    File: ScanConverter.cpp
    Description: spectrum data to base64-encoded string converter.
    Date: July 25, 2007

    Copyright (C) 2007 Joshua Tasman, ISB Seattle


    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

*/

#define LIBMSR_SOURCE
#include "ScanConverter.h"
#include <cmath>
#include "zlib.h"

using namespace std;

// for zlib:
#define CHECK_ERR(err, msg) {						\
	if (err != Z_OK) {								\
	fprintf(stderr, "%s error: %d\n", msg, err);	\
	exit(1);										\
	}												\
}

template<class T>
void SetEndianType(T& var, HostEndianType targetEndianType)
{
	static HostEndianType sourceEndianType = GetHostEndianType();
	static size_t typeSize = sizeof(T);
	static size_t typeHalfSize = typeSize/2u;

	if( sourceEndianType != targetEndianType )
	{
		unsigned char* varBytes = reinterpret_cast<unsigned char*>(&var);
		for(size_t i=0; i < typeHalfSize; ++i)
			std::swap(varBytes[i], varBytes[typeSize-i-1]);
	}
}

template<class DataPointType>
void FillMZRatioArray(ScanConverter& sc, HostEndianType targetEndianType, bool tryCompression_)
{
	int& mzRatioArrayLength_ = sc.mzRatioArrayLength_;
	bool& isMZRatioCompressed_ = sc.isMZRatioCompressed_;
	uLong& mzRatioArrayCompressedByteSize_ = sc.mzRatioArrayCompressedByteSize_;
	char*& mzRatioB64String_ = sc.mzRatioB64String_;
	int& mzRatioB64StringEncodedByteLen_ = sc.mzRatioB64StringEncodedByteLen_;
	Scan*& scan = sc.scan_;

	// handle zero length case
	if (scan->getNumDataPoints() == 0) {
		sc.mzRatioArrayLength_ = 0;
		sc.isMZRatioCompressed_ = false;
		sc.mzRatioArrayCompressedByteSize_ = 0;
		sc.mzRatioB64StringEncodedByteLen_ = 0;
		sc.mzRatioB64String_ = strdup("");
	}
	else {
		mzRatioArrayLength_ = scan->getNumDataPoints();
		DataPointType* mzRatioArray = new DataPointType[mzRatioArrayLength_];

		// copy actual data
		for (long c=0; c<mzRatioArrayLength_; c++) {
			mzRatioArray[c] = (DataPointType) scan->mzArray_[c];
		}

		// convert each float to network byte order
		for (long c=0; c<mzRatioArrayLength_; c++)
			SetEndianType(mzRatioArray[c], targetEndianType);

		// zlib compression
		Byte* mzRatioCompressedArray = NULL;

		if (tryCompression_) {
			int err;

			// compression buffer must be at least 0.1% of the size of the
			// data to be compressed + 12 bytes
			mzRatioArrayCompressedByteSize_ = (mzRatioArrayLength_ * sizeof(DataPointType));
			mzRatioArrayCompressedByteSize_ += (uLong)ceil(mzRatioArrayCompressedByteSize_ * 0.1) + 12;
			mzRatioCompressedArray = new Byte[mzRatioArrayCompressedByteSize_];
			if (mzRatioCompressedArray == NULL) {
				cerr << "Error allocating memory for peaks compression\n";
				exit(1);
			}

			err = compress(
				mzRatioCompressedArray,
				&mzRatioArrayCompressedByteSize_,
				(const Bytef*) mzRatioArray,
				mzRatioArrayLength_ * sizeof(DataPointType));
			if (err != Z_OK) {
				cerr << "compression error" << endl;
				exit(1);
			};
			isMZRatioCompressed_ = true;
		}

		// do base64 encoding
		Base64 base64Encoder;

		if (isMZRatioCompressed_) {
			mzRatioB64StringEncodedByteLen_ = base64Encoder.b64_encode(
				(const unsigned char *) mzRatioCompressedArray, 
				mzRatioArrayCompressedByteSize_);	
			delete mzRatioCompressedArray;
		}
		else {
			mzRatioB64StringEncodedByteLen_ = base64Encoder.b64_encode(
				(const unsigned char *) mzRatioArray, 
				(mzRatioArrayLength_ * sizeof(DataPointType)) );
		}
		mzRatioB64String_ = new char[mzRatioB64StringEncodedByteLen_ + 1]; // +1 for \0
		strcpy(mzRatioB64String_, (const char*)base64Encoder.getOutputBuffer());
		base64Encoder.freeOutputBuffer();

		// cleanup
		delete mzRatioArray;
	}
}

template<class DataPointType>
void FillIntensityArray(ScanConverter& sc, HostEndianType targetEndianType, bool tryCompression_)
{
	int& intensityArrayLength_ = sc.intensityArrayLength_;
	bool& isIntensityCompressed_ = sc.isIntensityCompressed_;
	uLong& intensityArrayCompressedByteSize_ = sc.intensityArrayCompressedByteSize_;
	char*& intensityB64String_ = sc.intensityB64String_;
	int& intensityB64StringEncodedByteLen_ = sc.intensityB64StringEncodedByteLen_;
	Scan*& scan = sc.scan_;

	// handle zero length case
	if (scan->getNumDataPoints() == 0) {
		intensityArrayLength_ = 0;
		isIntensityCompressed_ = false;
		intensityArrayCompressedByteSize_ = 0;
		intensityB64StringEncodedByteLen_ = 0;
		intensityB64String_ = strdup("");
	}
	else {
		intensityArrayLength_ = scan->getNumDataPoints();
		DataPointType* intensityArray = new DataPointType[intensityArrayLength_];

		// copy actual data
		for (long c=0; c<intensityArrayLength_; c++) {
			intensityArray[c] = (DataPointType) scan->intensityArray_[c];
		}

		// convert each float to network byte order
		for (long c=0; c<intensityArrayLength_; c++)
			SetEndianType(intensityArray[c], targetEndianType);

		// zlib compression
		Byte* intensityCompressedArray = NULL;

		if (tryCompression_) {
			int err;

			// compression buffer must be at least 0.1% of the size of the
			// data to be compressed + 12 bytes
			intensityArrayCompressedByteSize_ = 
				(intensityArrayLength_ * sizeof(DataPointType));
			intensityArrayCompressedByteSize_ += (uLong)ceil(intensityArrayCompressedByteSize_ * 0.1) + 12;
			intensityCompressedArray = new Byte[intensityArrayCompressedByteSize_];
			if (intensityCompressedArray == NULL) {
				cerr << "Error allocating memory for peaks compression\n";
				exit(1);
			}

			err = compress(
				intensityCompressedArray,
				&intensityArrayCompressedByteSize_,
				(const Bytef*) intensityArray,
				intensityArrayLength_ * sizeof(DataPointType));
			if (err != Z_OK) {
				cerr << "compression error" << endl;
				exit(1);
			};
			isIntensityCompressed_ = true;
		}

		// do base64 encoding
		Base64 base64Encoder;

		if (isIntensityCompressed_) {
			intensityB64StringEncodedByteLen_ = base64Encoder.b64_encode(
				(const unsigned char *) intensityCompressedArray, 
				intensityArrayCompressedByteSize_);	
			delete intensityCompressedArray;
		}
		else {
			intensityB64StringEncodedByteLen_ = base64Encoder.b64_encode(
				(const unsigned char *) intensityArray, 
				(intensityArrayLength_ * sizeof(DataPointType)) );
		}
		intensityB64String_ = new char[intensityB64StringEncodedByteLen_ + 1]; // +1 for \0
		strcpy(intensityB64String_, (const char*)base64Encoder.getOutputBuffer());
		base64Encoder.freeOutputBuffer();

		// cleanup
		delete intensityArray;
	}
}
#include <cassert>
template<class DataPointType>
void FillMZIntArray(ScanConverter& sc, HostEndianType targetEndianType, bool tryCompression_)
{
	int& mzIntArrayLength_ = sc.mzIntArrayLength_;
	bool& isMZIntCompressed_ = sc.isMZIntCompressed_;
	uLong& mzIntArrayCompressedByteSize_ = sc.mzIntArrayCompressedByteSize_;
	char*& mzIntB64String_ = sc.mzIntB64String_;
	int& mzIntB64StringEncodedByteLen_ = sc.mzIntB64StringEncodedByteLen_;
	Scan*& scan = sc.scan_;

	// handle zero length case
	if (scan->getNumDataPoints() == 0) {
		mzIntArrayLength_ = 0;
		isMZIntCompressed_ = false;
		mzIntArrayCompressedByteSize_ = 0;
		mzIntB64StringEncodedByteLen_ = 0;
		mzIntB64String_ = strdup("");
	}
	else {
		mzIntArrayLength_ = scan->getNumDataPoints()*2;
		DataPointType* mzIntArray = new DataPointType[mzIntArrayLength_];

		// copy actual data
		long mzIntIndex = 0;
		for (long c=0; c<scan->getNumDataPoints(); c++) {
			mzIntArray[mzIntIndex++] = (DataPointType) scan->mzArray_[c];
			mzIntArray[mzIntIndex++] = (DataPointType) scan->intensityArray_[c];
		}

		// convert each float to network byte order
		for (long c=0; c<mzIntArrayLength_; c++)
			SetEndianType(mzIntArray[c], targetEndianType);

		// zlib compression
		Byte* mzIntCompressedArray = NULL;

		if (tryCompression_) {
			int err;

			// compression buffer must be at least 0.1% of the size of the
			// data to be compressed + 12 bytes
			mzIntArrayCompressedByteSize_ = 
				(mzIntArrayLength_ * sizeof(DataPointType));
			mzIntArrayCompressedByteSize_ += (uLong)ceil(mzIntArrayCompressedByteSize_ * 0.1) + 12;
			mzIntCompressedArray = new Byte[mzIntArrayCompressedByteSize_];
			if (mzIntCompressedArray == NULL) {
				cerr << "Error allocating memory for peaks compression\n";
				exit(1);
			}

			err = compress(
				mzIntCompressedArray,
				&mzIntArrayCompressedByteSize_,
				(const Bytef*) mzIntArray,
				mzIntArrayLength_ * sizeof(DataPointType));
			if (err != Z_OK) {
				cerr << "compression error" << endl;
				exit(1);
			};
			isMZIntCompressed_ = true;
		}

		// do base64 encoding
		Base64 base64Encoder;

		if (isMZIntCompressed_) {
			mzIntB64StringEncodedByteLen_ = base64Encoder.b64_encode(
				(const unsigned char *) mzIntCompressedArray, 
				mzIntArrayCompressedByteSize_);	
			delete mzIntCompressedArray;
		}
		else {
			mzIntB64StringEncodedByteLen_ = base64Encoder.b64_encode(
				(const unsigned char *) mzIntArray, 
				(mzIntArrayLength_ * sizeof(DataPointType)) );
		}
		mzIntB64String_ = new char[mzIntB64StringEncodedByteLen_ + 1]; // +1 for \0
		strcpy(mzIntB64String_, (const char*)base64Encoder.getOutputBuffer());
		base64Encoder.freeOutputBuffer();

		// cleanup
		delete mzIntArray;
	}
}


template<class DataPointType>
void DecodeMZRatioArray(ScanConverter& sc, HostEndianType sourceEndianType, bool isCompressed)
{
	int& mzRatioArrayLength_ = sc.mzRatioArrayLength_;
	//bool& isMZRatioCompressed_ = sc.isMZRatioCompressed_;
	uLong& mzRatioArrayCompressedByteSize_ = sc.mzRatioArrayCompressedByteSize_;
	char*& mzRatioB64String_ = sc.mzRatioB64String_;
	int& mzRatioB64StringEncodedByteLen_ = sc.mzRatioB64StringEncodedByteLen_;
	Scan*& scan = sc.scan_;

	// do base64 decoding
	Base64 base64Decoder;

	int mzRatioDecodedByteLen = base64Decoder.b64_decode(mzRatioB64String_, mzRatioB64StringEncodedByteLen_);
	unsigned char* mzRatioDecodedArray = base64Decoder.getOutputBuffer();
	mzRatioArrayLength_ = scan->getNumDataPoints();
	DataPointType* mzRatioArray;

	if (isCompressed) {
		int err;
		mzRatioArrayCompressedByteSize_ = (uLong) strlen((const char*) mzRatioDecodedArray);
		const Bytef* mzRatioCompressedArray = (const Bytef*) mzRatioDecodedArray;
		mzRatioArray = new DataPointType[mzRatioArrayLength_];

		err = uncompress((Bytef*) mzRatioArray, (uLongf*) &mzRatioArrayCompressedByteSize_, 
						mzRatioCompressedArray, mzRatioArrayCompressedByteSize_);
		if (err != Z_OK) {
			cerr << "decompression error" << endl;
			exit(1);
		}
	} else
	{
		mzRatioArray = (DataPointType*) mzRatioDecodedArray;
		int mzRatioDecodedArrayLength = mzRatioDecodedByteLen / sizeof(DataPointType);
		if(mzRatioDecodedArrayLength < mzRatioArrayLength_)
		{
			cerr << "decoded mzRatio array is too small for the specified peak count (" <<
					mzRatioDecodedArrayLength << "<" << mzRatioArrayLength_ << ")" << endl;
			exit(1);
		}
	}

	// convert each float to the host's byte order
	for (long c = 0; c < mzRatioArrayLength_; ++c)
		SetEndianType(mzRatioArray[c], sourceEndianType);

	// copy actual data
	for (long c = 0; c < mzRatioArrayLength_; ++c) {
		scan->mzArray_[c] = (Scan::MzValueType) mzRatioArray[c];
	}

	// cleanup
	if (isCompressed)
		delete mzRatioArray;
	base64Decoder.freeOutputBuffer();
	mzRatioB64String_ = NULL;
}

template<class DataPointType>
void DecodeIntensityArray(ScanConverter& sc, HostEndianType sourceEndianType, bool isCompressed)
{
	int& intensityArrayLength_ = sc.intensityArrayLength_;
	//bool& isIntensityCompressed_ = sc.isIntensityCompressed_;
	uLong& intensityArrayCompressedByteSize_ = sc.intensityArrayCompressedByteSize_;
	char*& intensityB64String_ = sc.intensityB64String_;
	int& intensityB64StringEncodedByteLen_ = sc.intensityB64StringEncodedByteLen_;
	Scan*& scan = sc.scan_;

	// do base64 decoding
	Base64 base64Decoder;

	int intensityDecodedByteLen = base64Decoder.b64_decode(intensityB64String_, intensityB64StringEncodedByteLen_);
	unsigned char* intensityDecodedArray = base64Decoder.getOutputBuffer();
	intensityArrayLength_ = scan->getNumDataPoints();
	DataPointType* intensityArray;

	if (isCompressed) {
		int err;
		intensityArrayCompressedByteSize_ = (uLong) strlen((const char*) intensityDecodedArray);
		const Bytef* intensityCompressedArray = (const Bytef*) intensityDecodedArray;
		intensityArray = new DataPointType[intensityArrayLength_];

		err = uncompress((Bytef*) intensityArray, (uLongf*) &intensityArrayCompressedByteSize_, 
						intensityCompressedArray, intensityArrayCompressedByteSize_);
		if (err != Z_OK) {
			cerr << "decompression error" << endl;
			exit(1);
		}
	} else
	{
		intensityArray = (DataPointType*) intensityDecodedArray;
		int intensityDecodedArrayLength = intensityDecodedByteLen / sizeof(DataPointType);
		if(intensityDecodedArrayLength < intensityArrayLength_)
		{
			cerr << "decoded intensity array is too small for the specified peak count (" <<
					intensityDecodedArrayLength << "<" << intensityArrayLength_ << ")" << endl;
			exit(1);
		}
	}

	// convert each float to the host's byte order
	for (long c = 0; c < intensityArrayLength_; ++c)
		SetEndianType(intensityArray[c], sourceEndianType);

	// copy actual data
	for (long c = 0; c < intensityArrayLength_; ++c) {
		scan->intensityArray_[c] = (Scan::IntenValueType) intensityArray[c];
	}

	// cleanup
	if (isCompressed)
		delete intensityArray;
	base64Decoder.freeOutputBuffer();
	intensityB64String_ = NULL;
}

template<class DataPointType>
void DecodeMZIntArray(ScanConverter& sc, HostEndianType sourceEndianType, bool isCompressed)
{
	int& mzIntArrayLength_ = sc.mzIntArrayLength_;
	//bool& isMZIntCompressed_ = sc.isMZIntCompressed_;
	uLong& mzIntArrayCompressedByteSize_ = sc.mzIntArrayCompressedByteSize_;
	char*& mzIntB64String_ = sc.mzIntB64String_;
	int& mzIntB64StringEncodedByteLen_ = sc.mzIntB64StringEncodedByteLen_;
	Scan*& scan = sc.scan_;

	// do base64 decoding
	Base64 base64Decoder;

	int mzIntDecodedByteLen = base64Decoder.b64_decode(mzIntB64String_, mzIntB64StringEncodedByteLen_);
	unsigned char* mzIntDecodedArray = base64Decoder.getOutputBuffer();
	mzIntArrayLength_ = scan->getNumDataPoints() * 2;
	DataPointType* mzIntArray;

	if (isCompressed) {
		int err;
		mzIntArrayCompressedByteSize_ = (uLong) mzIntArrayLength_ * sizeof(DataPointType);
		const Bytef* mzIntCompressedArray = (const Bytef*) mzIntDecodedArray;
		mzIntArray = new DataPointType[mzIntArrayLength_];

		err = uncompress((Bytef*) mzIntArray, (uLongf*) &mzIntArrayCompressedByteSize_, 
						mzIntCompressedArray, mzIntDecodedByteLen);
		if (err != Z_OK) {
			cerr << "decompression error" << endl;
			exit(1);
		}
	} else
	{
		mzIntArray = (DataPointType*) mzIntDecodedArray;
		int mzIntDecodedArrayLength = mzIntDecodedByteLen / sizeof(DataPointType);
		if(mzIntDecodedArrayLength < mzIntArrayLength_)
		{
			cerr << "decoded mzInt array is too small for the specified peak count (" <<
					mzIntDecodedArrayLength << "<" << mzIntArrayLength_ << ")" << endl;
			exit(1);
		}
	}

	// convert each float to the host's byte order
	for (long c = 0; c < mzIntArrayLength_; c++)
		SetEndianType(mzIntArray[c], sourceEndianType);

	// copy actual data
	for (long c = 0, dataPointIndex = 0; c < mzIntArrayLength_; c += 2, ++dataPointIndex) {
		scan->mzArray_[dataPointIndex] = (Scan::MzValueType) mzIntArray[c];
		scan->intensityArray_[dataPointIndex] = (Scan::IntenValueType) mzIntArray[c+1];
	}

	// cleanup
	if (isCompressed)
		delete mzIntArray;
	base64Decoder.freeOutputBuffer();
	mzIntB64String_ = NULL;
}

ScanConverter::ScanConverter(
							Scan *scan,
							bool tryCompression,
							bool mzRatioDoublePrecision,
							bool intensityDoublePrecision,
							HostEndianType targetEndianType)
							 :
							mzRatioArrayLength_(-1),
							intensityArrayLength_(-1),
							mzIntArrayLength_(-1),
							isMZRatioCompressed_(false),
							isIntensityCompressed_(false),
							isMZIntCompressed_(false),
							mzRatioB64String_(NULL),
							intensityB64String_(NULL),
							mzIntB64String_(NULL),
							mzRatioB64StringEncodedByteLen_(-1),
							intensityB64StringEncodedByteLen_(-1),
							mzIntB64StringEncodedByteLen_(-1),
							scan_(scan),
							tryCompression_(tryCompression)
{
	if(mzRatioDoublePrecision)
		FillMZRatioArray<double>(*this, targetEndianType, tryCompression);
	else
		FillMZRatioArray<float>(*this, targetEndianType, tryCompression);

	if(intensityDoublePrecision)
		FillIntensityArray<double>(*this, targetEndianType, tryCompression);
	else
		FillIntensityArray<float>(*this, targetEndianType, tryCompression);

	if(mzRatioDoublePrecision || intensityDoublePrecision)
		FillMZIntArray<double>(*this, targetEndianType, tryCompression);
	else
		FillMZIntArray<float>(*this, targetEndianType, tryCompression);
}

ScanConverter::ScanConverter(
							Scan *scan,
							const char* mzRatioB64String,
							const char* intensityB64String,
							bool mzRatioIsCompressed,
							bool intensityIsCompressed,
							bool mzRatioDoublePrecision,
							bool intensityDoublePrecision,
							HostEndianType mzRatioEndianType,
							HostEndianType intensityEndianType)
							 :
							mzRatioArrayLength_(-1),
							intensityArrayLength_(-1),
							mzIntArrayLength_(-1),
							isMZRatioCompressed_(mzRatioIsCompressed),
							isIntensityCompressed_(intensityIsCompressed),
							isMZIntCompressed_(false),
							mzRatioB64String_((char*)mzRatioB64String),
							intensityB64String_((char*)intensityB64String),
							mzIntB64String_(NULL),
							mzIntB64StringEncodedByteLen_(-1),
							scan_(scan),
							tryCompression_(false)
{
	if(scan->getNumDataPoints() == 0)
	{
		mzRatioB64String_ = NULL;
		intensityB64String_ = NULL;
		return;
	}

	if(mzRatioB64String != NULL)
	{
		mzRatioB64StringEncodedByteLen_ = (uLong) strlen(mzRatioB64String);
		if(mzRatioDoublePrecision)
			DecodeMZRatioArray<double>(*this, mzRatioEndianType, mzRatioIsCompressed);
		else
			DecodeMZRatioArray<float>(*this, mzRatioEndianType, mzRatioIsCompressed);
	}

	if(intensityB64String != NULL)
	{
		intensityB64StringEncodedByteLen_ = (uLong) strlen(intensityB64String);
		if(intensityDoublePrecision)
			DecodeIntensityArray<double>(*this, intensityEndianType, intensityIsCompressed);
		else
			DecodeIntensityArray<float>(*this, intensityEndianType, intensityIsCompressed);
	}
}

ScanConverter::ScanConverter(
							Scan *scan,
							const char* mzIntB64String,
							bool isCompressed,
							bool isDoublePrecision,
							HostEndianType sourceEndianType)
							:
							mzRatioArrayLength_(-1),
							intensityArrayLength_(-1),
							mzIntArrayLength_(-1),
							isMZRatioCompressed_(false),
							isIntensityCompressed_(false),
							isMZIntCompressed_(isCompressed),
							mzRatioB64String_(NULL),
							intensityB64String_(NULL),
							mzIntB64String_((char*)mzIntB64String),
							mzRatioB64StringEncodedByteLen_(-1),
							intensityB64StringEncodedByteLen_(-1),
							scan_(scan),
							tryCompression_(false)
{
	if(scan->getNumDataPoints() == 0)
	{
		mzIntB64String_ = NULL;
		return;
	}

	mzIntB64StringEncodedByteLen_ = (uLong) strlen(mzIntB64String);

	if(isDoublePrecision)
		DecodeMZIntArray<double>(*this, sourceEndianType, isCompressed);
	else
		DecodeMZIntArray<float>(*this, sourceEndianType, isCompressed);
}

ScanConverter::~ScanConverter() {
	// cleanup
	SAFE_DELETE(mzRatioB64String_);
	SAFE_DELETE(intensityB64String_);
	SAFE_DELETE(mzIntB64String_);
}
