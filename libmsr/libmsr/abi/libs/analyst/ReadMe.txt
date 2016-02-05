trans_proteomic_pipeline\src\mzXML\converters\mzWiff\libs\analyst\ReadMe.txt

This directory contains the Analyst DLLs to be used by #import directive
via a relative path to generate .tlh and .tli files.

These DLLs are:

    1. AcqMethodSvr.dll
    2. ExploreDataObjects.dll
    3. FileManager.dll
    4. MSMethodSvr.dll
    5. Peak_Finder2.dll
    6. PeakFinderFactory.dll
    7. WIFFTransSvr.dll

You should NOT commit these DLLs into the source code repository.

Instead, go thru' each of the .tlh files and change the full path of 
#include "<.tli file>" to a relative path.

Please commit both the .tlh and .tli files that you have generated, so that 
other developers can use them to compile mzWiff.

