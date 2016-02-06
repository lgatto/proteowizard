#include "pwiz_tools/common/FullReaderList.hpp"
#include "pwiz/data/msdata/SpectrumInfo.hpp"
#include "pwiz/data/msdata/MSDataFile.hpp"
#include "pwiz/utility/misc/Std.hpp"

using namespace pwiz::msdata;

int main(int argc, const char * argv[]) {
  // The FullReaderList object enables reading of all supported file types.
  ReaderPtr readers(new FullReaderList);
  // The MSDataFile is an MSData object that handles file I/O. Here 
  // we pass the file name we wish to open, along with a list of all 
  // standard format readers to load the data, into the constructor.
  
  MSDataFile msd(argv[1], 0);
  // Here we get the number of Spectrum objects in a file.
  size_t numSpectra = msd.run.spectrumListPtr ->size();
  cout << "Run ID: " << msd.run.id << "; "
       << numSpectra << " spectra" << endl;
  
  for (size_t i=0; i < numSpectra; ++i) {
    // Retrieve the spectrum from the SpectrumList object, with binary data
    const bool getBinaryData = true;

    SpectrumPtr s = msd.run.spectrumListPtr->spectrum(i , getBinaryData);

    // Fetching the data loaded by the call to SpectrumList::spectrum()
    vector< double >& mzArray = s->getMZArray()-> data;
    vector< double >& intensityArray = s->getIntensityArray()->data;
    stringstream extrema;
    extrema << mzArray.front() << "," << intensityArray.front() << " "
	    << mzArray.back() << "," << intensityArray.back();
    cout << "Spectrum ID: " << s->id << "; "
	 << s->defaultArrayLength << " data points; "
	 << extrema << endl;
  }
  return 0;
}
