#include "pwiz/data/identdata/IdentDataFile.hpp"
#include "pwiz/data/identdata/IdentData.hpp"
#include <iostream>

using namespace pwiz::identdata;
using namespace std;

// void test(const char* filename) {
void test(const string& filename) {
  cout << "reading file in from " << filename << endl;
  
  IdentDataFile mzid(filename);
  cout << mzid.creationDate << endl;
  cout << mzid.provider.name << endl;
  cout << mzid.provider.id << endl;
  cout << mzid.analysisSoftwareList[0] << endl;

  // IdentDataFile *mzid;
  // mzid = new IdentDataFile(filename);
  // cout << mzid->creationDate << endl;
  
}

int main(int argc, char* argv[]) {
    try {
      if (argc < 2) throw runtime_error("Usage: hello_pwiz filename");
      test(argv[1]);
      return 0;
    } catch (exception& e) {
      cerr << e.what() << endl;
    } catch (...) {
      cerr << "Caught unknown exception.\n";
    }
    return 1;
}
