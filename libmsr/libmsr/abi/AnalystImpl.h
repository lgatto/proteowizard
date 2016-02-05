// -*- mode: c++ -*-


/*
    File: AnalystImpl.h
    Description: shared implementation across Analyst & AnalystQS interfaces
    Date: July 31, 2007

    Copyright (C) 2007 Chee Hong WONG, Bioinformatics Institute


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


#pragma once

#include <string>
#include "MSTypes.h"

#ifndef LIBMSR_PROJECT_PATH
#define LIBMSR_PROJECT_PATH "."
#endif

#define MAKE_TLI_PATH(leaf) LIBMSR_PROJECT_PATH "/"##leaf

enum LibraryTagType
{
	LIBRARY_Analyst = 0,
	LIBRARY_AnalystQS = 1
};

enum VerboseModeType
{
	VERBOSE_NONE = 0,
	VERBOSE_INFO = 1,       // provide informational log
	VERBOSE_DEBUG = 2,      // provide debug logs
	VERBOSE_DEBUGDETAIL = 3 // provide full detail debug log
};

int getLibraryVersion (enum LibraryTagType whichLibrary, std::string &strVersion);
const char *getLibraryName (enum LibraryTagType whichLibrary);
bool isCompatible (enum LibraryTagType whichLibrary, std::string &strSignature);
void getSoftwareVersion (const std::string &strSignature, std::string &strSoftware, std::string &strVersion);
MSAcquisitionSoftwareType softwareToMSAcquisitionSoftwareType(std::string &strSoftware);
int getModelById (long lId, std::string &strModel);
MSInstrumentModelType modelIdToMSInstrumentModel(long lId);
void getSampleLogEntry(const std::string &strSampleLog, const std::string &strKey, std::string &strValue);

long getMSLevel(long lScanType);
const char *getPolarityString(long lPolarity);
const char *getScanTypeString(long lScanType);
MSScanType getMSScanType(long lScanType);
inline bool isMRMScan(long lScanType)
{
	return (4 == lScanType);
}

inline bool isMSScanTypeMergable(MSScanType msScanType)
{
	//TODO: what else can we merged?
	return (TOFMS2==msScanType || EPI==msScanType);
}

typedef std::pair<long,long> SamplePeriodKey;

#include <vector>

class MergeCandidate {
public:
	long   m_lExperimentIndex;
	long   m_lDataPointIndex;
	double m_dPrecursorMZ;
	int    m_nPrecursorCharge;
	long   m_lScanNumber;
	bool   m_fMerged;
public:
	MergeCandidate()
	{
		m_lExperimentIndex = 0;
		m_lDataPointIndex = 0;
		m_dPrecursorMZ = 0;
		m_nPrecursorCharge = 0;
		m_lScanNumber = 0;
		m_fMerged = false;
	}
	MergeCandidate(long lExperimentIndex, long lDataPointIndex, double dPrecursorMZ, int nPrecursorCharge, long lScanNumber)
	{
		m_lExperimentIndex = lExperimentIndex;
		m_lDataPointIndex = lDataPointIndex;
		m_dPrecursorMZ = dPrecursorMZ;
		m_nPrecursorCharge = nPrecursorCharge;
		m_lScanNumber = lScanNumber;
		m_fMerged = false;
	}
	inline void dump(void) const
	{
		std::cout << "MergeCandidate(" << m_lExperimentIndex << ": " << m_lDataPointIndex;
		std::cout << ", " << m_dPrecursorMZ;
		std::cout << ", " << m_nPrecursorCharge;
		std::cout << ", " << m_lScanNumber;
		std::cout << ", " << (m_fMerged ? "merged" : "avail");
		std::cout << ")";
		std::cout << std::endl;
	}
};

typedef std::vector<MergeCandidate> MergeCandidates;

class MergedScan {
public:
	long                m_lExperimentIndex;
	long                m_lDataPointIndex;
	double              m_dPrecursorMZ;
	int                 m_nPrecursorCharge;
	long                m_lScanNumber;
	std::vector<long>   m_mergedExperimentIndex;
	std::vector<long>   m_mergedDataPointIndex;
	std::vector<long>   m_mergedScanNumber;
	std::vector<size_t> m_mergedScanCandidateIndex;

public:
	MergedScan() { init (); }
	MergedScan(const MergeCandidate &firstScan, size_t candidateId)
	{
		m_lExperimentIndex = firstScan.m_lExperimentIndex;
		m_lDataPointIndex = firstScan.m_lDataPointIndex;
		m_dPrecursorMZ = firstScan.m_dPrecursorMZ;
		m_nPrecursorCharge = firstScan.m_nPrecursorCharge;
		m_lScanNumber = firstScan.m_lScanNumber;
		mergeScan(firstScan, candidateId);
	}
	inline void init()
	{
		m_lExperimentIndex = 0;
		m_lDataPointIndex = 0;
		m_dPrecursorMZ = 0;
		m_nPrecursorCharge = 0;
		m_lScanNumber = 0;
		m_mergedExperimentIndex.resize(0);
		m_mergedDataPointIndex.resize(0);
		m_mergedScanNumber.resize(0);
		m_mergedScanCandidateIndex.resize(0);
	}
	inline void mergeScan(const MergeCandidate &scanToMerge, size_t candidateId)
	{
		m_mergedExperimentIndex.push_back(scanToMerge.m_lExperimentIndex);
		m_mergedDataPointIndex.push_back(scanToMerge.m_lDataPointIndex);
		m_mergedScanNumber.push_back(scanToMerge.m_lScanNumber);
		m_mergedScanCandidateIndex.push_back(candidateId);
	}
	inline size_t getNumberOfMergedScan() const
	{
		return m_mergedExperimentIndex.size();
	}
	inline size_t getCandidateId(int i) const
	{
		return m_mergedScanCandidateIndex[i];
	}
	inline long getMergedScanNumber(int i) const
	{
		return m_mergedScanNumber[i];
	}
};

class MergedScanTable {
public:
	typedef std::vector<MergedScan> MergedScans;

public:
	long        m_lSampleIndex;
	long        m_lPeriodIndex;
	MergedScans m_experimentsMergedScans;

public:
	MergedScanTable() { init(); }
	~MergedScanTable() { }

	inline SamplePeriodKey getKey(void) const
	{
		return SamplePeriodKey(m_lSampleIndex, m_lPeriodIndex);
	}
	inline void init(void)
	{
		initNewSample(0, 0);
	}
	inline void initNewSample(long lSampleIndex, long lPeriodIndex)
	{
		m_lSampleIndex = lSampleIndex;
		m_lPeriodIndex = lPeriodIndex;
		m_experimentsMergedScans.resize(0);
	}
	inline void cacheSampleMergedScan(const MergedScan &mergedScan)
	{
		m_experimentsMergedScans.push_back(mergedScan);
	}
	inline size_t getNumberOfMergedScan() const
	{
		return m_experimentsMergedScans.size();
	}
	inline void dump(void) const
	{
		if (m_experimentsMergedScans.size()>0) {
			std::cout << "#id: expt#, dp#, scan#, MZ, charge {expt#1, dp#1, scan#1; expt#2, dp#2, scan#2; ...}" << std::endl;
			long lIndex=0;
			for(MergedScans::const_iterator iter=m_experimentsMergedScans.begin();
				iter != m_experimentsMergedScans.end();
				++iter) {
				lIndex++;
				std::cout << "Merged Scan#" << lIndex << ": " << iter->m_lExperimentIndex;
				std::cout << ", " << iter->m_lDataPointIndex;
				std::cout << ", " << iter->m_lScanNumber;
				std::cout << ", " << iter->m_dPrecursorMZ;
				std::cout << ", " << iter->m_nPrecursorCharge;
				std::cout << " {";
				for (size_t i=0; i<iter->getNumberOfMergedScan(); i++) {
					if (i!=0) std::cout << ";";
					std::cout << " " << iter->m_mergedExperimentIndex[i];
					std::cout << "," << iter->m_mergedDataPointIndex[i];
					std::cout << "," << iter->m_mergedScanNumber[i];
				}
				std::cout << "}" << std::endl;
			}
		}
	}
};

#include <map>
typedef std::map<SamplePeriodKey,MergedScanTable > MergedScanTableCache;

#include "Scan.h"
#include <deque>
typedef Scan * ScanPtr;

class ScanIteratorState {
public:
	enum IteratorStateType
	{
    STATE_NO_TRANSITION = 0,
    STATE_ENDED = 1,
		STATE_NEW_SAMPLE = 2,
		STATE_NEW_PERIOD = 3,
		STATE_MRM_CONTINUE = 4,
		STATE_MERGEDSCAN_CONTINUE = 5
	};

	long   m_lCurrentSampleIndex;
	long   m_lCurrentPeriodIndex;
	long   m_lCurrentExperimentIndex;
	long   m_lCurrentDatapointIndex;
	long   m_lMaxSampleIndex;
	long   m_lMaxPeriodIndex;
	long   m_lMaxExperimentIndex;
	long   m_lMaxDatapointIndex;
	bool   m_fIterationEnded;

	bool                m_fMRMScans;
	std::deque<ScanPtr> m_queueMRMScans;

	bool                m_fMergedScans;
	bool                m_fServeMergedScans;
	IteratorStateType   m_stateAfterMerge;
	std::deque<ScanPtr> m_queueMergedScans;

public:
	ScanIteratorState() { init(); }
	~ScanIteratorState() { clearMRMScan(); }

	inline SamplePeriodKey getKey(void) const
	{
		return SamplePeriodKey(m_lCurrentSampleIndex, m_lCurrentPeriodIndex);
	}
	//Sample index starts with 1, Data point index starts with 1
	//Period index starts with 0, Experiment index starts with 0
	inline void init(void)
	{
		m_fIterationEnded = false;
		m_lCurrentSampleIndex = 1;
		m_lCurrentPeriodIndex = 0;
		m_lCurrentExperimentIndex = 0;
		m_lCurrentDatapointIndex = 1;
		clearMRMScan();
		clearMergedScan();
	}
	inline void initNewWiff(long lMaxSample, long lMaxPeriod, long lMaxExperiment)
	{
		m_lMaxSampleIndex = lMaxSample;
		m_lMaxPeriodIndex = lMaxPeriod;
		m_lMaxExperimentIndex = lMaxExperiment;
		clearMRMScan();
		clearMergedScan();
	}
	inline void initNewSample(long lMaxPeriod, long lMaxExperiment)
	{
		m_lMaxPeriodIndex = lMaxPeriod;
		m_lMaxExperimentIndex = lMaxExperiment;
		clearMRMScan();
		clearMergedScan();
	}
	inline void initNewPeriod(long lMaxExperiment)
	{
		m_lMaxExperimentIndex = lMaxExperiment;
		clearMRMScan();
		clearMergedScan();
	}
	inline void initNewDatapoints(long lMaxDatapoint)
	{
		m_lMaxDatapointIndex = lMaxDatapoint;
		clearMRMScan();
		clearMergedScan();
	}
	inline bool isEnd(void)
	{
		return m_fIterationEnded;
	}
	inline void pushMRMScan(Scan *pScan)
	{
		m_queueMRMScans.push_back(pScan);
		m_fMRMScans = true;
	}
	inline Scan *popMRMScan(void)
	{
		if (m_fMRMScans) {
			Scan *pScan = m_queueMRMScans.front(); 
			m_queueMRMScans.pop_front();
			m_fMRMScans = (m_queueMRMScans.size() > 0);
			return pScan;
		} else {
			return NULL;
		}
	}
	inline void clearMRMScan(void)
	{
		std::deque<ScanPtr>::const_iterator iter = m_queueMRMScans.begin();
		for (; iter != m_queueMRMScans.end(); ++iter) {
			delete *iter;
		}
		m_queueMRMScans.resize(0);
		m_fMRMScans = false;
	}
	inline bool isMRMScanAvailable(void)
	{
		return m_fMRMScans;
	}
	//-----
	inline void pushMergedScan(Scan *pScan)
	{
		m_queueMergedScans.push_back(pScan);
		m_fMergedScans = true;
	}
	inline Scan *popMergedScan(void)
	{
		if (m_fServeMergedScans && m_fMergedScans) {
			Scan *pScan = m_queueMergedScans.front(); 
			m_queueMergedScans.pop_front();
			m_fMergedScans = (m_queueMergedScans.size() > 0);
			return pScan;
		} else {
			return NULL;
		}
	}
	inline void clearMergedScan(void)
	{
		std::deque<ScanPtr>::const_iterator iter = m_queueMergedScans.begin();
		for (; iter != m_queueMergedScans.end(); ++iter) {
			delete *iter;
		}
		m_queueMergedScans.resize(0);
		m_fMergedScans = false;
		m_fServeMergedScans = false;
		m_stateAfterMerge = STATE_NO_TRANSITION;
	}
	inline bool isMergedScanAvailable(void)
	{
		return m_fMergedScans;
	}
	//-----
	IteratorStateType next(void)
	{
		if (m_fIterationEnded && !m_fServeMergedScans) return STATE_ENDED;

		if (!m_queueMRMScans.empty ()) return STATE_MRM_CONTINUE;

		if (m_fServeMergedScans) {
			if (!m_queueMergedScans.empty ()) return STATE_MERGEDSCAN_CONTINUE;

			m_fServeMergedScans = false;
			return m_stateAfterMerge;
		}

		m_lCurrentExperimentIndex++;
		if (m_lCurrentExperimentIndex >= m_lMaxExperimentIndex) {
			m_lCurrentExperimentIndex = 0;
			m_lCurrentDatapointIndex++;
			if (m_lCurrentDatapointIndex > m_lMaxDatapointIndex) { // data point is 1-based
				m_lCurrentDatapointIndex = 1;
				m_lCurrentPeriodIndex++;
				if (m_lCurrentPeriodIndex >= m_lMaxPeriodIndex) {
					m_lCurrentPeriodIndex = 0;
					m_lCurrentSampleIndex++;
					if (m_lCurrentSampleIndex > m_lMaxSampleIndex) { // sample is 1-based
						if (isMergedScanAvailable()) {
							m_fServeMergedScans = true;
							m_fIterationEnded = true;
							m_stateAfterMerge = STATE_ENDED;
							return STATE_MERGEDSCAN_CONTINUE;
						} else {
							m_fIterationEnded = true;
							return STATE_ENDED;
						}
					}
					// we have moved to a new sample
					// number of periods, number of experiments, and number of data points can changed
					if (isMergedScanAvailable()) {
						m_fServeMergedScans = true;
						m_stateAfterMerge = STATE_NEW_SAMPLE;
						return STATE_MERGEDSCAN_CONTINUE;
					} else {
						return STATE_NEW_SAMPLE;
					}
				} else {
					// we have moved to a new period
					// number of experiments, and number of data points can changed
					if (isMergedScanAvailable()) {
						m_fServeMergedScans = true;
						m_stateAfterMerge = STATE_NEW_PERIOD;
						return STATE_MERGEDSCAN_CONTINUE;
					} else {
						return STATE_NEW_PERIOD;
					}
				}
			}
		}
		return STATE_NO_TRANSITION;
	}
};

class ScanTable {
	typedef std::vector<long>    ScanNumbers;

public:
	bool m_fScanMapped;
	long m_lSampleIndex;
	long m_lPeriodIndex;
	long m_lNumExperiments;
	long m_lNumDataPoints;
	long m_lLastScanNumber;
	long m_lStartScanNumber;
	std::vector<ScanNumbers> m_experimentsScanNumbers;

public:
	ScanTable() { init(); }
	~ScanTable() { }

	inline SamplePeriodKey getKey(void) const
	{
		return SamplePeriodKey(m_lSampleIndex, m_lPeriodIndex);
	}
	inline void init(void)
	{
		m_fScanMapped = false;
		m_lSampleIndex = 0;
		m_lPeriodIndex = 0;
		m_lNumExperiments = 0;
		m_lNumDataPoints = 0;
		m_lStartScanNumber = m_lLastScanNumber = 0;
	}
	inline bool isAvailable(long lSampleIndex, long lPeriodIndex)
	{
		return (m_fScanMapped);
	}
	inline long getNumberOfExperiments()
	{
		return m_lNumExperiments;
	}
	inline long getNumberOfDataPoints()
	{
		return m_lNumDataPoints;
	}
	inline void initNewSample(long lSampleIndex, long lPeriodIndex, long lNumExperiments, long lNumDataPoints, long lLastScanNumber)
	{
		m_fScanMapped = false;
		m_experimentsScanNumbers.resize(lNumExperiments);
		for (long lExperimentIndex=0; lExperimentIndex<lNumExperiments; lExperimentIndex++) {
			m_experimentsScanNumbers[lExperimentIndex].resize(lNumDataPoints+1, 0);
		}
		m_lNumExperiments = lNumExperiments;
		m_lNumDataPoints = lNumDataPoints;
		m_lStartScanNumber = m_lLastScanNumber = lLastScanNumber;
	}
	inline void cacheSampleScanNumber(long lExperimentIndex, long lDataPointIndex)
	{
		m_lLastScanNumber++;
		m_experimentsScanNumbers[lExperimentIndex][lDataPointIndex]=m_lLastScanNumber;
	}
	inline void sampleCached(long lSampleIndex, long lPeriodIndex)
	{
		m_fScanMapped = true;
		m_lSampleIndex = lSampleIndex;
		m_lPeriodIndex = lPeriodIndex;
	}
	inline void getPrecursorScanNumber(long lExperimentIndex, long lDatapointIndex, long &lScanNumber) const
	{
		if (m_fScanMapped) {
			lScanNumber = m_experimentsScanNumbers[lExperimentIndex][lDatapointIndex];
		} else {
			lScanNumber = 0;
		}
	}
	inline void reorderScanNumbers()
	{
		if (!m_fScanMapped) return;

		long lCurrentScanNumber = m_lStartScanNumber;
		for (long lDataPointIndex=1; lDataPointIndex<=m_lNumDataPoints; lDataPointIndex++) {
			for (long lExperimentIndex=0; lExperimentIndex<m_lNumExperiments; lExperimentIndex++) {
				if (0!=m_experimentsScanNumbers[lExperimentIndex][lDataPointIndex]) {
					m_experimentsScanNumbers[lExperimentIndex][lDataPointIndex] = ++lCurrentScanNumber;
				}
			}
		}
	}
	inline long getLastScanNumberAssigned() const
	{
		return m_lLastScanNumber;
	}
};

class ScanOriginToMergeTable {
	typedef std::map<long, long> ScanOriginToMergeEntries;

public:
	ScanOriginToMergeEntries m_scanOriginToMergeEntries;

	ScanOriginToMergeTable() { }
	~ScanOriginToMergeTable() { }

	inline void add(long lScanOriginNum, long lMergedScanNum)
	{
		m_scanOriginToMergeEntries[lScanOriginNum] = lMergedScanNum;
	}
	inline long getMergedScanNum(long lScanOriginNum) const
	{
		std::map<long, long>::const_iterator iter = m_scanOriginToMergeEntries.find(lScanOriginNum);
		if (iter == m_scanOriginToMergeEntries.end()) {
			return lScanOriginNum;
		} else {
			return iter->second;
		}
	}
};

class DDEData {
	typedef std::vector<double>  PrecursorMasses;
	typedef std::vector<double>  PrecursorIntensities;
	typedef std::vector<int>     PrecursorCharges;
	typedef std::vector<double>  PrecursorCEs;

public:
	//TODO: flag for caching scan number
	bool m_fDDERealTimeData;
	bool m_fDDERealTimeDataEx;
	long m_lSampleIndex;
	long m_lPeriodIndex;
	long m_lNumExperiments;
	long m_lNumDataPoints;
	std::vector<PrecursorMasses>      m_experimentsPrecursorMasses;
	std::vector<PrecursorIntensities> m_experimentsPrecursorIntensities;
	std::vector<PrecursorCharges>     m_experimentsPrecursorCharges;
	std::vector<PrecursorCEs>         m_experimentsPrecursorCEs;

public:
	DDEData() { init(); }
	~DDEData() { }

	inline SamplePeriodKey getKey(void) const
	{
		return SamplePeriodKey(m_lSampleIndex, m_lPeriodIndex);
	}
	inline void init(void)
	{
		m_fDDERealTimeData = false;
		m_fDDERealTimeDataEx = false;
		m_lSampleIndex = 0;
		m_lPeriodIndex = 0;
		m_lNumExperiments = 0;
		m_lNumDataPoints = 0;
	}
	enum DDEDataType
	{
    TYPE_NONE = 0,
    TYPE_DDE = 1,
		TYPE_DDEEX = 2
	};
	inline DDEDataType getType(void)
	{
		if (m_fDDERealTimeDataEx) return TYPE_DDEEX;
		if (m_fDDERealTimeData) return TYPE_DDE;
		return TYPE_NONE;
	}
	inline bool isAvailable(long lSampleIndex, long lPeriodIndex)
	{
		return (m_fDDERealTimeData 
			&& lSampleIndex == m_lSampleIndex 
			&& lPeriodIndex == m_lPeriodIndex );
	}
	inline long getNumberOfExperiments()
	{
		return m_lNumExperiments;
	}
	inline long getNumberOfDataPoints()
	{
		return m_lNumDataPoints;
	}
	inline void initNewSample(long lSampleIndex, long lPeriodIndex, long lNumExperiments, long lNumDataPoints)
	{
		m_fDDERealTimeData = false;
		m_experimentsPrecursorMasses.resize(lNumExperiments);
		m_experimentsPrecursorIntensities.resize(lNumExperiments);
		m_experimentsPrecursorCharges.resize(lNumExperiments);
		m_experimentsPrecursorCEs.resize(lNumExperiments);
		for (long lExperimentIndex=0; lExperimentIndex<lNumExperiments; lExperimentIndex++) {
			m_experimentsPrecursorMasses[lExperimentIndex].resize(lNumDataPoints+1,0);
			m_experimentsPrecursorIntensities[lExperimentIndex].resize(lNumDataPoints+1, 0);
			m_experimentsPrecursorCharges[lExperimentIndex].resize(lNumDataPoints+1, 0);
			m_experimentsPrecursorCEs[lExperimentIndex].resize(lNumDataPoints+1, 0);
		}
		m_lNumExperiments = lNumExperiments;
		m_lNumDataPoints = lNumDataPoints;
	}
	inline void cacheSampleInfo(long lExperimentIndex, long lDataPointIndex, double dPrecursorMZ, double dPrecursorIntensity)
	{
		m_experimentsPrecursorMasses[lExperimentIndex][lDataPointIndex]=dPrecursorMZ;
		m_experimentsPrecursorIntensities[lExperimentIndex][lDataPointIndex]=dPrecursorIntensity;
	}
	inline void cacheSampleInfoEx(long lExperimentIndex, long lDataPointIndex, int nPrecursorCharge, double dPrecursorCE)
	{
		m_experimentsPrecursorCharges[lExperimentIndex][lDataPointIndex]=nPrecursorCharge;
		m_experimentsPrecursorCEs[lExperimentIndex][lDataPointIndex]=dPrecursorCE;
	}
	inline void sampleCached(long lSampleIndex, long lPeriodIndex)
	{
		m_fDDERealTimeData = true;
		m_lSampleIndex = lSampleIndex;
		m_lPeriodIndex = lPeriodIndex;
	}
	inline void sampleExCached(long lSampleIndex, long lPeriodIndex)
	{
		m_fDDERealTimeDataEx = true;
	}
	inline void getPrecursorInfo(long lExperimentIndex, long lDatapointIndex, double &dPrecursorMZ, double &dPrecursorIntensity)
	{
		if (m_fDDERealTimeData) {
			dPrecursorMZ = m_experimentsPrecursorMasses[lExperimentIndex][lDatapointIndex];
			dPrecursorIntensity = m_experimentsPrecursorIntensities[lExperimentIndex][lDatapointIndex];
		} else {
			dPrecursorMZ = dPrecursorIntensity = 0;
		}
	}
	inline void getPrecursorInfoEx(long lExperimentIndex, long lDatapointIndex, int &nPrecursorCharge, double &dPrecursorCE)
	{
		if (m_fDDERealTimeDataEx) {
			nPrecursorCharge = m_experimentsPrecursorCharges[lExperimentIndex][lDatapointIndex];
			dPrecursorCE = m_experimentsPrecursorCEs[lExperimentIndex][lDatapointIndex];
		} else {
			nPrecursorCharge = 0;
			dPrecursorCE = 0;
		}
	}
	// for use by mergeScans
	inline void getPrecursorMZCharge(long lExperimentIndex, long lDatapointIndex, double &dPrecursorMZ, double &dPrecursorIntensity, int &nPrecursorCharge)
	{
		if (m_fDDERealTimeData) {
			dPrecursorMZ = m_experimentsPrecursorMasses[lExperimentIndex][lDatapointIndex];
			if (m_fDDERealTimeDataEx) {
				nPrecursorCharge = m_experimentsPrecursorCharges[lExperimentIndex][lDatapointIndex];
				dPrecursorIntensity = m_experimentsPrecursorIntensities[lExperimentIndex][lDatapointIndex];
			} else {
				nPrecursorCharge = 0;
			}
		} else {
			dPrecursorMZ = 0;
			nPrecursorCharge = 0;
		}
	}
	// END - for use by mergeScans
	inline void dump(void) const
	{
		for (long lExperimentIndex=1; lExperimentIndex<m_lNumExperiments; lExperimentIndex++) {
			std::cout << (lExperimentIndex>1?",":"") << "Point,mz,intensity,charge,CE";
		}
		std::cout << std::endl;
		for (long lDataPointIndex=1; lDataPointIndex<=m_lNumDataPoints; lDataPointIndex++) {
			for (long lExperimentIndex=1; lExperimentIndex<m_lNumExperiments; lExperimentIndex++) {
				std::cout << (lExperimentIndex>1?",":"") << lExperimentIndex << ":" << lDataPointIndex;
				std::cout << "," << m_experimentsPrecursorMasses[lExperimentIndex][lDataPointIndex];
				std::cout << "," << m_experimentsPrecursorIntensities[lExperimentIndex][lDataPointIndex];
				std::cout << "," << m_experimentsPrecursorCharges[lExperimentIndex][lDataPointIndex];
				std::cout << "," << m_experimentsPrecursorCEs[lExperimentIndex][lDataPointIndex];
			}
			std::cout << std::endl;
		}
	}
};

class MSDataProcessing {
public:
	double m_dPeakCutoff;
	double m_dPeakCutoffPercentage;

	double m_dGroupPrecursorMassTolerance;
	int    m_iGroupMaxCyclesBetween;
	int    m_iGroupMinCycles;
	//future work
	bool   m_fGuessCharge;
	//END-future work

	int    m_iFilterMinPeakCount;

	bool   m_fCoordinate;

public:
	MSDataProcessing() { init(getDefaultProcessingOptions()); }
	~MSDataProcessing() { }

	inline void init(const ProcessingOptions& options)
	{
		m_fGuessCharge = false;
		m_fCoordinate = false;

		m_dPeakCutoff = options.get<double>("pic");
		m_dPeakCutoffPercentage = options.get<double>("picp");
		m_dGroupPrecursorMassTolerance = options.get<double>("gpmz");
		m_iGroupMaxCyclesBetween = options.get<int>("gpmax");
		m_iGroupMinCycles = options.get<int>("gpmin");
		m_iFilterMinPeakCount = options.get<int>("fpc");
	}

	static ProcessingOptions getDefaultProcessingOptions()
	{
		ProcessingOptions p;
		p["pic"] = ProcessingOption("pic", "real", "0.0", "Min. intensity to include a peak (tandem MS)", "(0,+inf]");
		p["picp"] = ProcessingOption("picp", "real", "10.0", "Min. percentage of most intense peak to include a peak (tandem MS)", "(0,100)");
		p["gpmz"] = ProcessingOption("gpmz", "real", "1.0", "Max. m/z error for a merged scan group", "(0,+inf]");
		p["gpmax"] = ProcessingOption("gpmax", "integer", "10", "Max. span of cycles for a merged scan group", "(1,+inf]");
		p["gpmin"] = ProcessingOption("gpmin", "integer", "1", "Min. span of cycles for a merged scan group", "(1,+inf]");
		p["fpc"] = ProcessingOption("fpc", "integer", "10", "Min. peak count to accept a tandem MS", "(0,+inf]");
		return p;
	}

	// for MRM
	inline double getPeakCutOff()
	{
		return (m_dPeakCutoff>0 ? m_dPeakCutoff : 0);
	}

	inline double getPeakCutOff(double dMaxIntensity)
	{
		if (m_dPeakCutoffPercentage>0) {
			return (m_dPeakCutoffPercentage*dMaxIntensity/100.0);
		}
		return (m_dPeakCutoff>0 ? m_dPeakCutoff : 0);
	}
};

