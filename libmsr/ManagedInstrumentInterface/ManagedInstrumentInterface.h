#include "stdafx.h"
#include "../libmsr/InstrumentInterface.h"
#using <mscorlib.dll>
using namespace System;
using namespace System::Collections::Generic;

#include <map>
using std::map;
using std::string;

#include <boost/function.hpp>

namespace ManagedLibMSR
{
	public enum class ScanTypeEnum {
		SCAN_UNDEF = 0,

		// originally added for Thermo
		FULL,		// full scan
		SIM,       	// single ion monitor
		SRM,       	// single reaction monitor
		CRM,       	// 
		Z,			// zoom scan
		Q1MS,		// q1 (quadrupole 1) mass spec scan
		Q3MS,		// q3 (quadrupole 3) mass spec scan

		// added for Waters

		// added for Analyst
		Q1Scan,
		Q1MI, 
		Q3Scan, 
		Q3MI, 
		MRM, 
		PrecursorScan,
		ProductIonScan, 
		NeutralLossScan, 
		TOFMS1, 
		TOFMS2, 
		TOFPrecursorIonScan,
		EPI, 
		ER, 
		MS3, 
		TDF, 
		EMS,
		EMC,

		// added for Agilent MassHunter

		NUM_SCAN_TYPES
	};

	struct PeakData
	{
		PeakData() : label(NULL), color(NULL), width(1) {}
		~PeakData() { if(label) delete label; if(color) delete color; }

		string* label;
		string* color;
		int width;
	};

	struct PeakDataMap : public map< double, PeakData >
	{
		PeakDataMap( Scan* scan = NULL )
		{
			if(scan)
				for( int i=0; i < scan->getNumDataPoints(); ++i )
					insert(end(), std::pair<double, PeakData>(scan->mzArray_[i], PeakData()));
		}

		iterator findNear( double mz, double tolerance )
		{
			iterator cur, min, max, best;

			min = lower_bound( mz - tolerance );
			max = lower_bound( mz + tolerance );

			if( min == max )
				return end(); // no peaks

			best = min;

			// find the peak closest to the desired mz
			double minDiff = (double) fabs( mz - best->first );
			for( cur = min; cur != max; ++cur )
			{
				double curDiff = (double) fabs( mz - cur->first );
				if( curDiff < minDiff )
				{
					minDiff = curDiff;
					best = cur;
				}
			}

			return best;
		}
	};

	public ref class ManagedPrecursorScanInfo
	{
	public: ManagedPrecursorScanInfo( PrecursorScanInfo* scan ) : m_scan(scan) {}
			~ManagedPrecursorScanInfo() { /* don't delete; that is handled by the native interface */ }

	private: PrecursorScanInfo* m_scan;

	public:
		property double ScanNumber { double get() { return m_scan->num_; } }
		property double MsLevel { double get() { return m_scan->msLevel_; } }
		property double Charge { double get() { return m_scan->charge_; } }
		property double Mz { double get() { return m_scan->MZ_; } }
		property double Intensity { double get() { return m_scan->intensity_; } }
		property double CollisionEnergy { double get() { return m_scan->collisionEnergy_; } }
	};

	public ref class ManagedScan
	{
	public: ManagedScan() : m_scan(new Scan()) {}
	public: ManagedScan( Scan* scan ) : m_scan(scan) {}
			~ManagedScan() { delete m_scan; }

	private: Scan* m_scan;

	public:
		property String^ SourceName { String^ get() { return gcnew String(m_scan->source_.c_str()); } }
		property long ScanNumber { long get() { return m_scan->num_; } }
		property int MsLevel { int get() { return m_scan->msLevel_; } }
		property double RetentionTime { double get() { return m_scan->retentionTimeInSec_; } }
		property ScanTypeEnum ScanType { ScanTypeEnum get() { return ScanTypeEnum( m_scan->scanType_ ); } }

		// last scan range
		property double MinScannedMz { double get() { return m_scan->startMZ_; } }
		property double MaxScannedMz { double get() { return m_scan->endMZ_; } }

		// all scan ranges
		//typedef pair<MzValueType, MzValueType> MzRange;
		//vector<MzRange> mzRanges_;

		property double MinObservedMz { double get() { return m_scan->minObservedMZ_; } }
		property double MaxObservedMz { double get() { return m_scan->maxObservedMZ_; } }

		// base peak: peak with greatest intesity
		property double BasePeakMz { double get() { return m_scan->basePeakMZ_; } }
		property double BasePeakIntensity { double get() { return m_scan->basePeakIntensity_; } }
		property double TotalIntensity { double get() { return m_scan->totalIonCurrent_; } }

		property bool IsCentroided { bool get() { return m_scan->isCentroided_; } }
		property bool IsDeisotoped { bool get() { return m_scan->isDeisotoped_; } }

		// for MSn >= 2
		int getPrecursorScanCount() { return m_scan->precursorList_.size(); }
		ManagedPrecursorScanInfo^ getPrecursorScanInfo(int index)
		{
			if( index < 0 || index >= (int) m_scan->precursorList_.size() )
				throw gcnew ArgumentException( "precursor scan index out of range" );
			return gcnew ManagedPrecursorScanInfo( &m_scan->precursorList_[index] );
		}

		void addPeak( double mz, double intensity ) { m_scan->addDataPoint( mz, intensity ); }

		int getTotalPeakCount() { return (int) m_scan->mzArray_.size(); }
		double getPeakMz(int index)
		{
			if( index < 0 || index >= (int) m_scan->mzArray_.size() )
				throw gcnew ArgumentException( "peak index out of range" );
			return m_scan->mzArray_[index];
		}

		double getPeakIntensity(int index)
		{
			if( index < 0 || index >= (int) m_scan->intensityArray_.size() )
				throw gcnew ArgumentException( "peak index out of range" );
			return m_scan->intensityArray_[index];
		}

	private: static String^ svgTemplateFilepath = L"SpectrumSVG-template.svg";
	public:
		static property String^ SpectrumSVGTemplateFilepath
		{
			String^ get() { return svgTemplateFilepath; }
			void set(String^ value) { svgTemplateFilepath = value; }
		}

	public: String^ writeToSvg()
		{
			return writeToSvg(nullptr, nullptr, nullptr);
		}

	public: String^ writeToSvg( SortedDictionary<double, String^>^ peakLabels )
		{
			return writeToSvg(peakLabels, nullptr, nullptr);
		}

	public: String^ writeToSvg( SortedDictionary<double, String^>^ peakLabels,
								SortedDictionary<double, String^>^ peakColors )
		{
			return writeToSvg(peakLabels, peakColors, nullptr);
		}

			// The peak attributes must be keyed to the peak m/z
	public: String^ writeToSvg( SortedDictionary<double, String^>^ peakLabels,
								SortedDictionary<double, String^>^ peakColors,
								SortedDictionary<double, int>^ peakWidths )
		{
			Scan* scan = m_scan;

			PeakDataMap peakDataMap;//(scan);
			if( peakLabels != nullptr )
			{
				for each( KeyValuePair<double, String^>^ itr in peakLabels )
				{
					PeakDataMap::iterator peakItr = peakDataMap.insert(pair<double, PeakData>(itr->Key, PeakData())).first;
					peakItr->second.label = new string(ToStdString(itr->Value));
				}
			}

			if( peakColors != nullptr )
			{
				for each( KeyValuePair<double, String^>^ itr in peakColors )
				{
					PeakDataMap::iterator peakItr = peakDataMap.insert(pair<double, PeakData>(itr->Key, PeakData())).first;
					peakItr->second.color = new string(ToStdString(itr->Value));
				}
			}

			if( peakWidths != nullptr )
			{
				for each( KeyValuePair<double, int>^ itr in peakWidths )
				{
					PeakDataMap::iterator peakItr = peakDataMap.insert(pair<double, PeakData>(itr->Key, PeakData())).first;
					peakItr->second.width = itr->Value;
				}
			}


			// Find the most intense peak in the observed scan range
			float MaxInten = 0;
			for( int i=0; i < scan->getNumDataPoints(); ++i )
				if( fabs( scan->intensityArray_[i] ) > MaxInten )
					MaxInten = fabs( scan->intensityArray_[i] );

			int maxPrecursorCharge = 0;

			std::ostringstream annotationsStream;
			annotationsStream << "[ ";
			bool hasFirstAnnotation = false;
			for( PeakDataMap::iterator itr = peakDataMap.begin(); itr != peakDataMap.end(); ++itr )
			{
				vector<string> optionVars;
				if( itr->second.width > 1 )
					optionVars.push_back( string( "width:" ) + lexical_cast<string>( itr->second.width ) );
				if( itr->second.label )
					optionVars.push_back( string( "label:\"" ) + *itr->second.label + "\"" );
				if( itr->second.color )
					optionVars.push_back( string( "color:\"" ) + *itr->second.color + "\"" );
				if( !optionVars.empty() )
				{
					annotationsStream	<< ( !hasFirstAnnotation ? "[" : ",[" )
										<< itr->first << ","
										<< 0 << ",{"
										<< optionVars[0];
					for( size_t j=1; j < optionVars.size(); ++j )
						annotationsStream << "," << optionVars[j];
					annotationsStream << "}]";
					hasFirstAnnotation = true;
				}
			}
			annotationsStream << " ]";

			std::ostringstream precursorAnnotationsStream;
			precursorAnnotationsStream << "[ ";
			for( size_t i=0; i < scan->precursorList_.size(); ++i )
			{
				maxPrecursorCharge = max( maxPrecursorCharge, scan->precursorList_[i].charge_ );
				precursorAnnotationsStream	<< ( i == 0 ? "[" : ",[" )
											<< scan->precursorList_[i].MZ_ << ","
											<< 0 << ",{"
											<< "dasharray:[1,2],width:3,height:-30,"
											<< "charge:" << scan->precursorList_[i].charge_ << ","
											<< "color:\"green\"}]";
			}
			precursorAnnotationsStream << " ]";

			string svgTemplateFilepath = ToStdString(ManagedScan::svgTemplateFilepath);
			/*static*/ ifstream svgTemplateFile(svgTemplateFilepath.c_str(), ios::binary);
			if( !svgTemplateFile )
				throw gcnew Exception(L"failed to find SpectrumSVG template");

			/*static*/ string svgTemplateString;
			if( svgTemplateString.empty() )
			{
				size_t svgTemplateSize = (size_t) System::IO::FileInfo(ManagedScan::svgTemplateFilepath).Length;
				svgTemplateString.resize( svgTemplateSize );
				svgTemplateFile.read( &svgTemplateString[0], svgTemplateSize );
			}

			std::ostringstream scanNameStream;
			scanNameStream << scan->source_ << '.' << scan->num_;

			std::ostringstream peaksAscendingByMzStream;
			peaksAscendingByMzStream << "[ ";

			multimap< double, double > peaksByMz;
			for( int i=0; i < scan->getNumDataPoints(); ++i )
				peaksByMz.insert( pair< double, double >( scan->mzArray_[i], scan->intensityArray_[i] ) );

			multimap< double, size_t > peaksByIntensity;
			size_t i = 1;
			for( multimap< double, double >::iterator itr2 = peaksByMz.begin(); itr2 != peaksByMz.end() && i < (1<<16); ++itr2, ++i )
			{
				peaksByIntensity.insert( pair< double, size_t >( itr2->second, i ) );

				peaksAscendingByMzStream	<< ( itr2 == peaksByMz.begin() ? "[" : ",[" )
											<< itr2->first << ","
											<< itr2->second;
				peaksAscendingByMzStream << "]";
			}
			peaksAscendingByMzStream << " ]";

			std::ostringstream peakIndexDescendingByIntensityStream;
			peakIndexDescendingByIntensityStream << "[";
			for( multimap< double, size_t >::reverse_iterator itr2 = peaksByIntensity.rbegin(); itr2 != peaksByIntensity.rend(); ++itr2 )
				peakIndexDescendingByIntensityStream << ( itr2 == peaksByIntensity.rbegin() ? "" : "," ) << itr2->second;
			peakIndexDescendingByIntensityStream << "]";

			double fragmentMzTolerance = 0.5;
			double isotopeMzTolerance = fragmentMzTolerance / 2.0;

			String^ svgOut = gcnew String( svgTemplateString.c_str() );
			svgOut = svgOut->Replace( L"%TRACK_NAVIGATION%", "true" );
			svgOut = svgOut->Replace( L"%PEAK_DRAW_METHOD%", scan->isCentroided_ ? "1" : "0" );
			svgOut = svgOut->Replace( L"%SCAN_NAME_STRING%", gcnew String(scanNameStream.str().c_str()) );
			svgOut = svgOut->Replace( L"%NUM_CHARGE_STATES%", Convert::ToString(maxPrecursorCharge) );
			svgOut = svgOut->Replace( L"%NUM_FRAGMENT_CHARGE_STATES%", Convert::ToString(maxPrecursorCharge-1) );
			svgOut = svgOut->Replace( L"%FRAGMENT_MZ_TOLERANCE%", Convert::ToString(fragmentMzTolerance) );
			svgOut = svgOut->Replace( L"%ISOTOPE_MZ_TOLERANCE%", Convert::ToString(isotopeMzTolerance) );
			svgOut = svgOut->Replace( L"%ANNOTATIONS%", gcnew String(annotationsStream.str().c_str()) );
			svgOut = svgOut->Replace( L"%PRECURSOR_ANNOTATIONS%", gcnew String(precursorAnnotationsStream.str().c_str()) );
			svgOut = svgOut->Replace( L"%PEAK_INDEX_DESCENDING_BY_INTENSITY%", gcnew String(peakIndexDescendingByIntensityStream.str().c_str()) );
			svgOut = svgOut->Replace( L"%PEAKS_ASCENDING_BY_MZ%", gcnew String(peaksAscendingByMzStream.str().c_str()) );

			return svgOut;
		}
	};

	public delegate bool ScanFilterDelegate( ManagedScan^ s );

	public ref class ManagedInstrumentInterface
	{
	public:
		ManagedInstrumentInterface()
			: pInterface(new InstrumentInterface), m_filterDelegate(nullptr)
		{}

		~ManagedInstrumentInterface()
		{
			try
			{
				pInterface->freeInterface();
				delete pInterface;
			} catch( Exception^ e )
			{
				throw gcnew Exception(e->Message);
			}
		}

		bool initInterface()
		{
			try
			{
				return pInterface->initInterface();
			} catch( exception& e )
			{
				throw gcnew Exception(gcnew String(e.what()));
			}
		}

		void freeInterface()
		{
			try
			{
				pInterface->freeInterface();
			} catch( Exception^ e )
			{
				throw gcnew Exception(e->Message);
			}
		}

		bool setInputFile(String^ fileName)
		{
			try
			{
				return pInterface->setInputFile( ToStdString( fileName ) );
			} catch( exception& e )
			{
				throw gcnew Exception(gcnew String(e.what()));
			}
		}

		//static ProcessingOptions getDefaultProcessingOptions(const string& formatExtension) { retu}
		//void setProcessingOptions(const ProcessingOptions& options) {}

		void setCentroiding(bool centroid, bool tandemCentroid)
		{
			pInterface->setCentroiding( centroid, tandemCentroid, true );
		}

		void setCentroiding(bool centroid, bool tandemCentroid, bool preferVendor)
		{
			pInterface->setCentroiding( centroid, tandemCentroid, preferVendor );
		}

		void setDeisotoping(bool deisotope) { pInterface->setDeisotoping(deisotope); }

		void setScanFilter( ScanFilterDelegate^ scanFilterDelegate )
		{
			m_filterDelegate = scanFilterDelegate;
		}

		long getFirstScanNumber()	{ return pInterface->getFirstScanNumber(); }
		long getLastScanNumber()	{ return pInterface->getLastScanNumber(); }
		long getTotalScanCount()	{ return pInterface->getTotalScanCount(); }

		ManagedScan^ getScan() // returns next available scan (first, initally)
		{
			Scan* scan = pInterface->getScan();
			while( m_filterDelegate != nullptr && scan != NULL && !nativeFilter(scan) )
			{
				delete scan;
				scan = pInterface->getScanHeader();
			}
			if( scan == NULL )
				return nullptr;
			return gcnew ManagedScan(scan);
		}

		ManagedScan^ getScanHeader() // same as above, but without populating the peak arrays
		{
			Scan* scan = pInterface->getScanHeader();
			while( m_filterDelegate != nullptr && scan != NULL && !nativeFilter(scan) )
			{
				delete scan;
				scan = pInterface->getScanHeader();
			}
			if( scan == NULL )
				return nullptr;
			return gcnew ManagedScan(scan);
		}

		ManagedScan^ getScan(long scanNumber) // returns requested scan by number, nullptr if unavailable
		{
			Scan* scan = pInterface->getScan(scanNumber);
			if( scan == NULL )
				return nullptr;
			else if( m_filterDelegate != nullptr && !nativeFilter(scan) )
			{
				delete scan;
				return nullptr;
			}
			return gcnew ManagedScan(scan);
		}

		ManagedScan^ getScanHeader(long scanNumber) // same as above, but without populating the peak arrays
		{
			Scan* scan = pInterface->getScanHeader(scanNumber);
			if( scan == NULL )
				return nullptr;
			else if( m_filterDelegate != nullptr && !nativeFilter(scan) )
			{
				delete scan;
				return nullptr;
			}
			return gcnew ManagedScan(scan);
		}

	public: ManagedScan^ getChromatogram()
		{
			Scan* chromatogram = new Scan;
			if( chromatogram == NULL )
				return nullptr;

			chromatogram->num_ = 0;
			chromatogram->retentionTimeInSec_ = 0;
			chromatogram->isCentroided_ = false;

			Scan* scan = pInterface->getScanHeader( pInterface->getFirstScanNumber() );
			while( m_filterDelegate != nullptr && scan != NULL && !nativeFilter(scan) )
			{
				delete scan;
				scan = pInterface->getScanHeader();
			}
			while( scan != NULL )
			{
				chromatogram->addDataPoint( scan->retentionTimeInSec_, scan->totalIonCurrent_ );
				//chromatogram->retentionTimeInSec_ += scan->retentionTimeInSec_;
				chromatogram->totalIonCurrent_ += scan->totalIonCurrent_;
				delete scan;
				scan = pInterface->getScanHeader();
				while( m_filterDelegate != nullptr && scan != NULL && !nativeFilter(scan) )
				{
					delete scan;
					scan = pInterface->getScanHeader();
				}
			}
			return gcnew ManagedScan(chromatogram);
		}

	private:
		InstrumentInterface* pInterface;
		ScanFilterDelegate^ m_filterDelegate;

		bool nativeFilter( Scan* scan )
		{
			return m_filterDelegate( gcnew ManagedScan(scan) );
		}
	};
}
