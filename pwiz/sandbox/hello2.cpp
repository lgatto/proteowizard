#include "pwiz_tools/common/MSDataAnalyzerApplication.hpp"
#include "pwiz/utility/misc/Std.hpp"

using namespace pwiz::data;
using namespace pwiz::cv;
using namespace pwiz::analysis;

class HelloAnalyzer : public MSDataAnalyzer {
public:

  virtual void open(const DataInfo& dataInfo) {
    cout << "sourceFilename: " << dataInfo.sourceFilename << endl;
    cout << "outputDirectory: " << dataInfo.outputDirectory << endl;
  }

  virtual UpdateRequest updateRequested(const DataInfo& dataInfo, 
					const SpectrumIdentity& spectrumIdentity) const {
    return UpdateRequest_NoBinary;
  }

  virtual void update(const DataInfo& dataInfo, 
		      const Spectrum& spectrum) {
    Scan dummy;
    const Scan& scan = spectrum.scanList.scans.empty() ? dummy : spectrum.scanList.scans[0];

    cout << "spectrum: " << spectrum.index << " " 
	 << spectrum.id << " "
	 << "ms" << spectrum.cvParam(MS_ms_level).value << " ["
	 << scan.cvParam(MS_scan_start_time).value << "] ["
	 << scan.cvParam(MS_ion_injection_time).value << "] ["
	 << scan.cvParam(MS_filter_string).value
	 << endl;
  }

  virtual void close(const DataInfo& dataInfo) {}
};


int main(int argc, const char* argv[]) {
  try {
    MSDataAnalyzerApplication app(argc, argv);

    if (app.filenames.empty()) {
      cout << "Usage: hello_analyzer [options] [filenames]\n"
	   << "Options:\n" << app.usageOptions << endl
	   << "http://proteowizard.sourceforge.net\n"
	   << "support@proteowizard.org\n";
      return 1;
    }

    HelloAnalyzer analyzer;
    app.run(analyzer, &cerr);

    return 0;
  } catch (exception& e) {
    cerr << e.what() << endl;
  } catch (...) {
    cerr << "Caught unknown exception.\n";
  }    
  return 1;
}


