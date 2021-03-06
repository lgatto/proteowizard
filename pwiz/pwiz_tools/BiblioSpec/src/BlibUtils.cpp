//
// $Id: BlibUtils.cpp 7209 2015-02-19 23:16:49Z kaipot $
//
//
// Original author: Barbara Frewen <frewen@u.washington.edu>
//
// Copyright 2012 University of Washington - Seattle, WA 98195
//
// Licensed under the Apache License, Version 2.0 (the "License"); 
// you may not use this file except in compliance with the License. 
// You may obtain a copy of the License at 
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software 
// distributed under the License is distributed on an "AS IS" BASIS, 
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. 
// See the License for the specific language governing permissions and 
// limitations under the License.
//


#include "BlibUtils.h"


using namespace std;
namespace bfs = boost::filesystem;

namespace BiblioSpec {

/**
 * String representations of score types.
 */
const char* scoreTypeNames[NUM_PSM_SCORE_TYPES] = {
    "UNKNOWN", 
    "PERCOLATOR QVALUE", 
    "PEPTIDE PROPHET SOMETHING",
    "SPECTRUM MILL",
    "IDPICKER FDR",
    "MASCOT IONS SCORE", 
    "TANDEM EXPECTATION VALUE", 
    "PROTEIN PILOT CONFIDENCE",
    "SCAFFOLD SOMETHING",
    "WATERS MSE PEPTIDE SCORE",
    "OMSSA EXPECTATION SCORE",
    "PROTEIN PROSPECTOR EXPECTATION SCORE",
    "SEQUEST XCORR",
    "MAXQUANT SCORE",
    "MORPHEUS SCORE",
    "MSGF+ SCORE",
    "PEAKS CONFIDENCE SCORE",
    "BYONIC SCORE",
    "PEPTIDE SHAKER CONFIDENCE"
};

/**
 * \brief Translate a string value into its corresponding score type.  
 * \returns The score type for the given string, UNKNOWN if string is
 * invalid.
 */
PSM_SCORE_TYPE stringToScoreType(const string& scoreName){
    PSM_SCORE_TYPE valFromString = UNKNOWN_SCORE_TYPE;

    for(int i = 0; i < NUM_PSM_SCORE_TYPES; i++){
        if( scoreName.compare(scoreTypeNames[i]) == 0 ){
            valFromString = (PSM_SCORE_TYPE)i;
            break;
        }
    }

    return valFromString;
}

/**
 * Returns the string representation of the score type.
 */
const char* scoreTypeToString(PSM_SCORE_TYPE scoreType){
    return scoreTypeNames[scoreType];
}

/**
 * \brief Return a string from the root to the given filename.
 * For filenames with no path, prepends current working directory.
 * Does not resolve symbolic links. Does not check that the file
 * exists.
 */
string getAbsoluteFilePath(string filename){
    bfs::path fullPath = bfs::system_complete(filename.c_str());
    fullPath = fullPath.normalize();

    return fullPath.string();
}

/*
This version resolves symbolic links, but is POSIX specific, doesn't
work on Windows.  I couldn't find the Windows equivilent.
#include <stdlib.h> // for realpath
string getAbsoluteFilePath(const char* filename){

    char buff[4096];
    realpath(filename, buff);
    // fullpath() Windows??
    return string(buff);
}
*/
/**
 * \brief Return all of the string before the last / or \\. Returns an
 * empty string if neither found.
 */
string getPath(string fullFileName)
{
    string filepath;
    // find the beginning of the file name
    size_t lastSlash = fullFileName.find_last_of("/\\");
    if( lastSlash != string::npos ) {
        filepath = fullFileName.substr(0, lastSlash+1);
    }

    return filepath;
}

/**
 * \brief Return all of string after the last '.'.  Returns an empty
 * string if none found.
 */
string getExtension(string fullFileName);


/**
 * \brief Return the string between the last \ or // and the last
 * '.'.  Returns the whole string if neither found.
 */
string getFileRoot(string fullFileName)
{
    string fileroot;

    // find the beginning of the filename
    size_t lastSlash = fullFileName.find_last_of("/\\");
    if( lastSlash == string::npos ) {// if not found, set to beginning
        lastSlash = 0;
    } else {
        lastSlash += 1;
    }

    // find the beginning of the extension
    size_t lastDot = fullFileName.find_last_of(".");
    fileroot = fullFileName.substr(lastSlash, lastDot - lastSlash);

    //  cerr << "file root of "<< fullFileName << " is " << fileroot << endl;
    return fileroot;
}

/**
 * \brief Returns true if the end of the filename matches exactly the
 * ext string.  Assumes the ext string includes a dot.
 */
bool hasExtension(string filename, string ext);
bool hasExtension(string filename, const char* ext){
    const char* name_ptr = filename.c_str();
    return strcmp(name_ptr + filename.size() - strlen(ext), ext) == 0;
}

bool hasExtension(const char* name, const char* ext) {
    return strcmp(name + strlen(name) - strlen(ext), ext) == 0;
}

bool compare_first_pair_doubles_descending(const pair<double, double>& left,
                                           const pair<double, double>& right){
    return (left.first < right.first);
}

/**
 * Compare two doubles for sorting in descending order
 */
bool doublesDescending(const double left, const double right){
    return left > right;
}

/**
 * Replace all occurances of findChar and replace them with
 * replaceChar, returning the number of substitutions performed.
 */
size_t replaceAllChar(string& str, const char findChar, const char replaceChar){
    size_t count = 0;
    size_t position = str.find_first_of(findChar);
    while(position != string::npos){
        str.at(position) = replaceChar;
        count++;
        position = str.find_first_of(findChar);
    }
    return count;
}

/**
 *  Replace all characters after the last '.' with ext.  If no '.'
 *  found concatinate .ext onto the filename. 
 */
void replaceExtension(string& filename, const char* ext){
    size_t lastDot = filename.find_last_of(".");
    if( lastDot == string::npos) {
        filename += ".";
    } else {
        filename.erase(lastDot + 1);
    }

    filename += ext;

    return;
}

/**
 * Sum the masses of amino acids and modifications from the given
 * array of masses (as initialized by AminoAcidMasses).
 */
double getPeptideMass(string& modifiedSeq, double* masses){
    double mass = 0;
    for(int i = 0; i < (int)modifiedSeq.size(); i++){

        char aa = modifiedSeq[i];

        if( aa == '[' ){
            size_t end = modifiedSeq.find_first_of("]", i);
            string modStr = modifiedSeq.substr(i + 1, end - i);
            double modMass = atof(modStr.c_str());
            i = end + 1;

            mass += modMass;
        } else if( aa >= 'A' && aa <= 'Z' ){
            mass += masses[(int)aa];
        } else {
            Verbosity::error("Illegal character %c for computing mass of %s.",
                             aa, modifiedSeq.c_str());
        }
    }
    return mass;
}


/**
 * Delete any spaces or tabs at the end of the given string
 */
void deleteTrailingWhitespace(string& str){
    size_t last = str.size() - 1;
    while( str.at(last) == ' ' || str.at(last) == '\t' ){
        last--;
    }
    // last is now index position of last non-whitespace char
    str.erase(last + 1);
}

/**
 * Create a copy of the given string, converting all characters to
 * upper case.
 */
char* strcpy_all_caps(const char* original){
    if( original == NULL ){
        return NULL;
    }
    char* new_str = new char[strlen(original)];
    strcpy(new_str, original);

    for(int i = 0; new_str[i]; i++ ){
        new_str[i] = toupper(new_str[i]);
    }

    return new_str;
}
#ifdef _MSC_VER
#include <Windows.h>
#else
#include <unistd.h>
#endif
/**
 * Return the full path to the location of the executable.
 */
string getExeDirectory(){
    string path;
    int size = 1024;
    char pathBuffer[1024];
    size_t len = -1;
    char slash = '/';
#ifdef _MSC_VER
    len = GetModuleFileName(NULL, pathBuffer, size);
    slash = '\\';
#else
    // NOTE: won't work for Solaris, FreeBSD or Mac OS X
    len = readlink("/proc/self/exe", pathBuffer, size);
#endif
    // remove the executable name at the end
    size_t curPos = len;
    while( len > 0 && pathBuffer[curPos] != slash ){
        curPos--;
    }
    if( len < 1 ){
        throw BlibException(false, 
                            "Could not find the location of this executable.");
    }
    pathBuffer[curPos + 1] = '\0';
    path = pathBuffer;
    return path;
}
} // namespace

/*
 * Local Variables:
 * mode: c
 * c-basic-offset: 4
 * End:
 */
