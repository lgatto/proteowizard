//
// hello_pwiz.cpp
//


#include "pwiz/data/msdata/RAMPAdapter.hpp"
#include "pwiz_tools/common/MSDataAnalyzerApplication.hpp"
#include "pwiz/data/msdata/MSDataFile.hpp"
#include "pwiz/data/msdata/Serializer_mzML.hpp"
#include "pwiz/data/msdata/Diff.hpp"
#include "pwiz/data/msdata/examples.hpp"
#include "pwiz/utility/misc/unit.hpp"
#include "pwiz/utility/misc/Std.hpp"
#include <iostream>


using namespace pwiz::data;
using namespace pwiz::msdata;
using namespace pwiz::util;
using namespace pwiz::cv;
using namespace pwiz::analysis;
using namespace std;


void test(const char* filename)
{
    RAMPAdapter adapter(filename);

    InstrumentStruct instrument;
    adapter.getInstrument(instrument);
    cout << "manufacturer: " << instrument.manufacturer << endl;
    cout << "model: " << instrument.model << endl;
    cout << "ionisation: " << instrument.ionisation << endl;
    cout << "analyzer : " << instrument.analyzer << endl;
    cout << "detector: " << instrument.detector << endl;

    size_t scanCount = adapter.scanCount();
    cout << "scanCount: " << scanCount << "\n\n";

    for (size_t i=0; i<scanCount; i++)
    {
        ScanHeaderStruct header;
        adapter.getScanHeader(i, header);

        cout << "index: " << i << endl;
        cout << "seqNum: " << header.seqNum << endl;
        cout << "acquisitionNum: " << header.acquisitionNum << endl;
        cout << "msLevel: " << header.msLevel << endl;
        cout << "peaksCount: " << header.peaksCount << endl;
        cout << "precursorScanNum: " << header.precursorScanNum << endl;
        cout << "totIonCurrent: " << header.totIonCurrent << endl;
        cout << "ionInjectionTime: " << header.ionInjectionTime << endl;
        cout << "filePosition: " << header.filePosition << endl;

        vector<double> peaks;
        adapter.getScanPeaks(i, peaks);

        for (unsigned int j=0; j<min(peaks.size(),(size_t)20); j+=2)
            cout << "  (" << peaks[j] << ", " << peaks[j+1] << ")\n";

        cout << endl;
    }
}


int main(int argc, char* argv[])
{
    try
    {
        if (argc<2) throw runtime_error("Usage: hello_ramp filename");
        test(argv[1]);
        return 0;
    }
    catch (exception& e)
    {
        cerr << e.what() << endl;
    }
    catch (...)
    {
        cerr << "Caught unknown exception.\n";
    }

    return 1;
}

