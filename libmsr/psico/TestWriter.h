#ifndef _INCLUDED_TESTWRITER_H_
#define _INCLUDED_TESTWRITER_H_

#include "common.h"
#include "MassSpecXMLWriter.h"

class TestWriter : public MassSpecXMLWriter {
public:
	TestWriter(BaseInstrumentInterface* testInterface)
		: MassSpecXMLWriter("", "", testInterface) {}
	~TestWriter() {}

	void writeDocument(void)
	{
		ostream& out = *pOut_;

		out << "instrument:name\t" << (int) instrumentInterface_->instrumentInfo_.instrumentModel_ << "\n";
		out << "instrument:source\t" << (int) instrumentInterface_->instrumentInfo_.ionSource_ << "\n";
		out << "instrument:analyzerList\t";
		for(size_t i=0; i < instrumentInterface_->instrumentInfo_.analyzerList_.size(); ++i)
			out << (int) instrumentInterface_->instrumentInfo_.analyzerList_[i] << " ";
		out << "\n";
		out << "instrument:detector\t" << (int) instrumentInterface_->instrumentInfo_.detector_ << "\n";
		out << "processing:deisotoping\t" << instrumentInterface_->doDeisotope_ << "\n";
		out << "processing:centroiding\t" << instrumentInterface_->doCentroid_ << "\n";

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

			out << curScan->num_ << ":scanType\t" << (int) curScan->scanType_ << "\n";
			out << curScan->num_ << ":retentionTimeInSeconds\t" << curScan->retentionTimeInSec_ << "\n";
			out << curScan->num_ << ":ionization\t" << (int) curScan->ionization_ << "\n";
			out << curScan->num_ << ":polarity\t" << (int) curScan->polarity_ << "\n";
			out << curScan->num_ << ":analyzer\t" << (int) curScan->analyzer_ << "\n";
			out << curScan->num_ << ":activation\t" << (int) curScan->activation_ << "\n";
			out << curScan->num_ << ":isCentroided\t" << curScan->isCentroided_ << "\n";
			out << curScan->num_ << ":isDeisotoped\t" << curScan->isDeisotoped_ << "\n";
			out << curScan->num_ << ":totalIonCurrent\t" << curScan->totalIonCurrent_ << "\n";
			out << curScan->num_ << ":msLevel\t" << curScan->msLevel_ << "\n";
			out << curScan->num_ << ":minObservedMZ\t" << curScan->minObservedMZ_ << "\n";
			out << curScan->num_ << ":maxObservedMZ\t" << curScan->maxObservedMZ_ << "\n";
			out << curScan->num_ << ":startMZ\t" << curScan->startMZ_ << "\n";
			out << curScan->num_ << ":endMZ\t" << curScan->endMZ_ << "\n";
			out << curScan->num_ << ":basePeakIntensity\t" << curScan->basePeakIntensity_ << "\n";
			out << curScan->num_ << ":basePeakMZ\t" << curScan->basePeakMZ_ << "\n";
			out << curScan->num_ << ":precursorList\t";
			for(size_t i=0; i < curScan->precursorList_.size(); ++i)
			{
				PrecursorScanInfo& p = curScan->precursorList_[i];
				out <<	"(" << p.num_ << "," << p.charge_ << "," << p.MZ_ << "," << p.msLevel_ << "," <<
						p.intensity_ << "," << p.collisionEnergy_ << /*"," << p.accurateMZ_ <<*/ ") ";
			}
			out << "\n";

			int numDataPoints = curScan->getNumDataPoints();
			curPeakCount += numDataPoints;

			out << curScan->num_ << ":peakMZs\t";
			for(int i=0; i < numDataPoints; ++i)
				out << curScan->mzArray_[i] << " ";
			out << "\n";
			out << curScan->num_ << ":peakIntensities\t";
			for(int i=0; i < numDataPoints; ++i)
				out << curScan->intensityArray_[i] << " ";
			out << "\n";

			delete curScan;
		}
	}

	bool setInputFile(const string& inputFileName)
	{
		inputFileName_ = inputFileName;
		return true;
	}
};

#endif
