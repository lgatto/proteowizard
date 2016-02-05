// -*- mode: c++ -*-


/*
    File: mzDataWriter.h
    Description: instrument-independent mzData writer.
    Date: October 12, 2007

    Copyright (C) 2007 Matt Chambers, Vanderbilt MSRC


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
#include "common.h"
#include "mzDataWriter.h"
#include "filesystem.h"

mzDataWriter::mzDataWriter(	const string& programName,
							const string& programVersion,
							BaseInstrumentInterface* BaseInstrumentInterface) : 
MassSpecXMLWriter(programName,
				  programVersion,
				  BaseInstrumentInterface)
{
}

bool
mzDataWriter::setInputFile(const string& inputFileName)
{
	inputFileName_ = inputFileName;
	return true;
}

void
mzDataWriter::writeDocument(void)
{
	condenseAttr_ = true;

	startDocument();

	open("mzData");
	attr("version", "1.05");
	attr("accessionNumber", "psi-ms:12345");
	{
		open("description");
		{
			open("admin");
			{
				open("sampleName");
					data("n/a");
				close(); // sampleName

				open("sampleDescription");
					data("n/a");
				close(); // sampleDescription

				open("sourceFile");
				{
					path filepath((inputFileNameList_.empty() ? inputFileName_ : *inputFileNameList_.begin()), filesystem::native);
					open("nameOfFile");
						data(filepath.leaf());
					close(); // nameOfFile

					open("pathToFile");
						data(convertToURI(filepath.branch_path().native_file_string(), hostName_));
					close(); // pathToFile

					open("fileType");
						data(extension(filepath).substr(1)); // trim leading dot from file extension
					close(); // fileType
				}
				close(); // sourceFile

				open("contact");
				{
					open("name");
					close(); // name

					open("institution");
					close(); // institution

					open("contactInfo");
					close(); // contactInfo
				}
				close(); // contact
			}
			close(); // admin

			open("instrument");
			{
				open("instrumentName"); // map to "instrument model"
					data(toString(instrumentInterface_->instrumentInfo_.instrumentModel_));
				close(); // instrumentName

				open("source");
					cvParam("psi", "PSI:1000008", "ionization type", toOBOText(instrumentInterface_->instrumentInfo_.ionSource_));
				close(); // source

				open("analyzerList");
				attr("count", instrumentInterface_->instrumentInfo_.analyzerList_.size());
				for(size_t i=0; i < instrumentInterface_->instrumentInfo_.analyzerList_.size(); ++i)
				{
					open("analyzer");
						cvParam("psi", "PSI:1000010", "analyzer type", toOBOText(instrumentInterface_->instrumentInfo_.analyzerList_[i]));
					close(); // analyzer
				}
				close(); // analyzerList

				open("detector");
					cvParam("psi", "PSI:1000026", "detector type", toString(instrumentInterface_->instrumentInfo_.detector_));
				close(); // detector
			}
			close(); // instrument

			open("dataProcessing");
			{
				open("software");
				{
					open("name");
						data(programName_);
					close(); // name
					open("version");
						data(programVersion_);
					close(); // version
				}
				close(); // software

				open("processingMethod");
				{
					cvParam("psi", "PSI:1000033", "deisotoping", (doDeisotoping_ ? "true" : "false"));
					cvParam("psi", "PSI:1000034", "charge deconvolution", "false");
					cvParam("psi", "PSI:1000035", "peak processing", (doCentroiding_ ? "discrete" : "continuous"));
				}
				close(); // processingMethod
			}
			close(); // dataProcessing
		}
		close(); // description

		open("spectrumList");
		attr("count", instrumentInterface_->totalNumScans_);
		size_t curScanCount = 0;
		size_t curPeakCount = 0;
		while(true)
		{
			Scan* curScan = instrumentInterface_->getScan();

			if (verbose_) {
				if ((curScanCount % 10) == 0 || curScan == NULL) {
					cout << curScanCount << " of " << instrumentInterface_->totalNumScans_ << " scans converted; " <<
							curPeakCount << " peaks written.\t\r";
				}
			}

			if (curScan == NULL) {
				if (verbose_ && curScanCount >= 10)
					cout << endl;
				break;
			}

			++curScanCount;

			if( curScan->msLevel_ < minMsLevel_ || curScan->msLevel_ > maxMsLevel_ ||
				curScan->num_ < minScanNum_ || curScan->num_ > maxScanNum_ )
			{
				delete curScan;
				continue;
			}

			open("spectrum");
			attr("id", curScan->num_);
			{
				open("spectrumDesc");
				{
					open("spectrumSettings");
					{
						open("acqSpecification");
						attr("spectrumType", (curScan->isCentroided_ ? "discrete" : "continuous"));
						attr("methodOfCombination", "sum");
						attr("count", 1);
						{
							open("acquisition");
							attr("acqNumber", curScan->num_);
							close(); // acquisition
						}
						close(); // acqSpecification

						open("spectrumInstrument");
						attr("msLevel", curScan->msLevel_);
						attr("mzRangeStart", curScan->startMZ_);
						attr("mzRangeStop", curScan->endMZ_);
						{
							// hybrid instruments need some love but who knows where this should really go
							if (curScan->analyzer_ != instrumentInterface_->instrumentInfo_.analyzerList_.back())
								cvParam("psi", "PSI:1000010", "analyzer type", toOBOText(curScan->analyzer_));

							cvParam("psi", "PSI:1000037", "polarity", (curScan->polarity_ == POSITIVE ? "positive" : "negative"));
							cvParam("psi", "PSI:1000039", "time in seconds", curScan->retentionTimeInSec_);

							// should this go here?
							cvParam("psi", "PSI:1000285", "total ion current", curScan->totalIonCurrent_);

							// what about this one?
							cvParam("psi", "PSI:1000210", "base peak", curScan->basePeakMZ_);

							// base peak intensity is not in the mzData CV
							open("userParam");
							attr("name", "base peak intensity");
							attr("value", curScan->basePeakIntensity_);
							close(); // userParam

							// scan type in mzData is not well understood; the official examples don't do it properly!
							open("userParam");
							attr("name", "scan type");
							attr("value", toString(curScan->scanType_));
							close(); // userParam

							// mzData CV doesn't support minObservedMZ and maxObservedMZ
							open("userParam");
							attr("name", "lowest observed m/z");
							attr("value", curScan->minObservedMZ_);
							close(); // userParam

							open("userParam");
							attr("name", "highest observed m/z");
							attr("value", curScan->maxObservedMZ_);
							close(); // userParam
						}
						close(); // spectrumInstrument
					}
					close(); // spectrumSettings

					if(curScan->msLevel_ >= 2)
					{
						open("precursorList");
						attr("count", curScan->precursorList_.size());
						{
							for(size_t i=0; i < curScan->precursorList_.size(); ++i)
							{
								open("precursor");
								attr("msLevel", (curScan->msLevel_-1)); // required
								attr("spectrumRef", curScan->precursorList_[i].num_); // required
								{
									open("ionSelection");
									{
										cvParam("psi", "PSI:1000040", "mass to charge ratio", curScan->precursorList_[i].MZ_);
										if(curScan->precursorList_[i].charge_ > 0)
											cvParam("psi", "PSI:1000041", "charge state", curScan->precursorList_[i].charge_);
										if(curScan->precursorList_[i].intensity_ > 0)
											cvParam("psi", "PSI:1000042", "intensity", curScan->precursorList_[i].intensity_);
									}
									close(); // ionSelection

									open("activation");
									{
										cvParam("psi", "PSI:1000044", "method", toOBOText(curScan->activation_));
										cvParam("psi", "PSI:1000045", "collision energy", curScan->precursorList_[i].collisionEnergy_);
									}
									close(); // activation
								}
								close(); // precursor
							}
						}
						close(); // precursorList
					}
				}
				close(); // spectrumDesc

				{
					static HostEndianType hostEndianType = GetHostEndianType();
					//static HostEndianType hostEndianType = COMMON_BIG_ENDIAN;
					ScanConverter scanConverter(curScan, false, mzValuesDoublePrecision_, intenValuesDoublePrecision_, hostEndianType); // will free mem on descope

					open("mzArrayBinary");
					{
						open("data");
						attr("precision", (mzValuesDoublePrecision_ ? "64" : "32"));
						attr("endian", (hostEndianType == COMMON_BIG_ENDIAN ? "big" : "little"));
						attr("length", scanConverter.mzRatioArrayLength_);
						data(scanConverter.mzRatioB64String_);
						close(); // data
					}
					close(); // mzArrayBinary

					open("intenArrayBinary");
					{
						open("data");
						attr("precision", (intenValuesDoublePrecision_ ? "64" : "32"));
						attr("endian", (hostEndianType == COMMON_BIG_ENDIAN ? "big" : "little"));
						attr("length", scanConverter.intensityArrayLength_);
						data(scanConverter.intensityB64String_);
						close(); // data
					}
					close(); // intenArrayBinary

					curPeakCount += curScan->getNumDataPoints();
				}
			}
			close(); // spectrum
			delete curScan;
		}
		close(); // spectrumList
	}
	close(); // mzData
}
