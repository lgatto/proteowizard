// -*- mode: c++ -*-


/*
    File: ScanConverter.h
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


#ifndef _INCLUDED_SCANCONVERTER_H_
#define _INCLUDED_SCANCONVERTER_H_

#include "common.h"
#include "Base64.h"
#include "zlib.h"
#include "Scan.h"

// one time base64 encoder and compression,
// frees memory on destruction

class ScanConverter {
public:
	// encode constructor
	ScanConverter(
		Scan* scan,
		bool tryCompression = false,
		bool mzRatioDoublePrecision = false,
		bool intensityDoublePrecision = false,
		HostEndianType targetEndianType = COMMON_BIG_ENDIAN );

	// decode constructor for separate arrays
	ScanConverter(
		Scan* scan,
		const char* mzRatioB64String,
		const char* intensityB64String,
		bool mzRatioIsCompressed = false,
		bool intensityIsCompressed = false,
		bool mzRatioDoublePrecision = false,
		bool intensityDoublePrecision = false,
		HostEndianType mzRatioEndianType = COMMON_BIG_ENDIAN,
		HostEndianType intensityEndianType = COMMON_BIG_ENDIAN );

	// decode constructor for single pairwise array
	ScanConverter(
		Scan* scan,
		const char* mzIntB64String,
		bool isCompressed = false,
		bool isDoublePrecision = false,
		HostEndianType sourceEndianType = COMMON_BIG_ENDIAN );

	~ScanConverter();

	int mzRatioArrayLength_;
	int intensityArrayLength_;
	int mzIntArrayLength_;

	bool isMZRatioCompressed_; // was zlib compression successful?
	bool isIntensityCompressed_; // was zlib compression successful?
	bool isMZIntCompressed_; // was zlib compression successful?

	uLong mzRatioArrayCompressedByteSize_;
	uLong intensityArrayCompressedByteSize_;
	uLong mzIntArrayCompressedByteSize_;

	char* mzRatioB64String_;
	char* intensityB64String_;
	char* mzIntB64String_;

	int mzRatioB64StringEncodedByteLen_;
	int intensityB64StringEncodedByteLen_;
	int mzIntB64StringEncodedByteLen_;

	Scan* scan_;

private:
	bool tryCompression_;
};

#endif
