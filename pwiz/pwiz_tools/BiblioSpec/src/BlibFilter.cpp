//
// $Id: BlibFilter.cpp 6870 2014-11-03 19:12:11Z chambm $
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

/*
 * This programs takes a redundant library and
 * compute all_vs_all, select one final spec into
 * the non_redundant library.
 *
 * $ LibFilter.cpp, v 1.0 2009/01/27 15:53:52 Ning Zhang Exp $
 */

#include <cstdio>
#include <iostream>
#include <cstdlib>
#include <fstream>
#include <string>
#include <sqlite3.h>
#include <map>
#include <vector>
#include <time.h>
#include "zlib.h"
#include "RefSpectrum.h"
#include "PeakProcess.h"
#include "DotProduct.h"
#include "Match.h"
#include "BlibMaker.h"
#include "ProgressIndicator.h"
#include "BlibUtils.h"
#include "Verbosity.h"
#include "CommandLine.h"
#include "SqliteRoutine.h"
#include "boost/program_options.hpp"

using namespace std;
namespace ops = boost::program_options;

namespace BiblioSpec {

class BlibFilter : public BlibMaker
{
 public:
    BlibFilter();
    ~BlibFilter();
    void parseCommandLine(const int argc, char** const argv,
                          ops::variables_map& options_table);
    void init();
    virtual void commit();
    virtual void attachAll();
    void buildNonRedundantLib();

 protected:
    virtual string getLSID();
    virtual void getNextRevision(int* major, int* minor);
    vector<PEAK_T> getUncompressedPeaks(int& numPeaks,
                                        int& mzLen, Byte* comprM, 
                                        int& intensityLen, Byte* comprI);
    void compAndInsert(vector<RefSpectrum*>& oneIon);
    map< int, vector<RefSpectrum*> > groupByScoreType(const vector<RefSpectrum*>& oneIon, map<RefSpectrum*, int>* outIndices);
    vector<RefSpectrum*> getBestScores(const vector<RefSpectrum*>& group, bool higherIsBetter);

 private:
    string redundantFileName_;  // The name of the file being filtered
    // filtered lib name stored by BlibMaker
    const char* redundantDbName_; // The name it's given as an attached db
    int minPeaks_;        // Spectrum must have this many peaks to be included
    double minAverageScore_; // don't include best spec if average dotp is lower

    int tableVersion_;
    bool useBestScoring_;
    map<int, bool> higherIsBetter_;
    char zSql[2048];

    void getCommandLineValues(ops::variables_map& options_table);
};
} // namespace

using namespace BiblioSpec;

/**
 * The starting point for BlibFilter, program to select one spectrum
 * for each peptide ion as the best representative of a redundant
 * library and store it in a new library.
 */
int main(int argc, char* argv[])
{
    // declare storage for options values
    ops::variables_map options_table;

    BlibFilter filter;
    filter.parseCommandLine(argc, argv, options_table);

    try {
        filter.init();

        filter.beginTransaction();
        Verbosity::debug("About to begin filtering.");
        filter.buildNonRedundantLib();
        Verbosity::debug("Finished filtering.");
        filter.endTransaction();
        filter.commit();
    } catch (BlibException& e) {
        cerr << "ERROR: " << e.what();
    } catch (exception& e) {
        cerr << "ERROR: " << e.what();
    } catch (...) {
        cerr << "ERROR: Unknown error";
    }
}

BlibFilter::BlibFilter()
{
    redundantDbName_ = "redundant";
    minPeaks_ = 20; 
    minAverageScore_ = 0;
    tableVersion_ = 0;
    useBestScoring_ = false;
    // Never append to a non-redundant library
    setOverwrite(true);
    setRedundant(false);
}

BlibFilter::~BlibFilter(){
}

/**
 * Read the given command line and store option and argument values in
 * the given variables_map.  Also reads parameter file with command
 * line values overriding file values.  Sets appropriate values for
 * the BlibFilter object (filenames, options).
 *
 * Exits on error if unexpected option found, if option is missing its
 * argument, if required argument is missing.
 */
void BlibFilter::parseCommandLine(const int argc, 
                                  char** const argv,
                                  ops::variables_map& options_table){

    // define the optional command line args 
    options_description optionsDescription("Options");
    try{

        optionsDescription.add_options()
            ("memory-cache,m",
             value<int>()->default_value(250),
             "SQLite memory cache size in Megs.  Default 250M.")

            ("min-peaks,n",
             value<int>()->default_value(1),
             "Only include spectra with at least this many peaks.  Default 1.")

            ("min-score,s",
             value<double>()->default_value(0),
             "Best spectrum must have at least this average score to be included.  Default 0.")

            ("best-scoring,b",
             value<bool>()->default_value(false),
             "Description of option.  Default false.")

            ;

        // define the required command line args
        vector<const char*> argNames;
        argNames.push_back("redundant-library");
        argNames.push_back("filtered-library");

        // create a CommandLine object to do the dirty work
        BiblioSpec::CommandLine parser("BlibFilter", 
                                       optionsDescription,
                                       argNames, 
                                       false);// no multiple values
                                              // for last arg

        // read command line and param file (if given)
        parser.parse(argc, argv, options_table);

    } catch(std::exception& e) {
        cerr << "ERROR: " << e.what() << "." << endl << endl;
        exit(1);
    } catch(...) {
        BiblioSpec::Verbosity::error("Encountered exception of unknown type "
                                     "while parsing command line.");
    }

    getCommandLineValues(options_table);
}

/**
 * Save the needed values parsed from the command line.
 */
void BlibFilter::getCommandLineValues(ops::variables_map& options_table){
    redundantFileName_ = options_table["redundant-library"].as<string>();
    minPeaks_ = options_table["min-peaks"].as<int>();
    minAverageScore_ = options_table["min-score"].as<double>();
    setLibName(options_table["filtered-library"].as<string>());
    useBestScoring_ = options_table["best-scoring"].as<bool>();
}

void BlibFilter::attachAll()
{
    Verbosity::status("Filtering redundant library '%s'.",
                      redundantFileName_.c_str());
    sprintf(zSql, "ATTACH DATABASE '%s' as %s", SqliteRoutine::ESCAPE_APOSTROPHES(redundantFileName_).c_str(), 
            redundantDbName_);
    sql_stmt(zSql);
}

void BlibFilter::commit()
{
    BlibMaker::commit();
    
    string detachCmd = "DETACH DATABASE ";
    detachCmd += redundantDbName_;
    sql_stmt(detachCmd.c_str());
}

string BlibFilter::getLSID()
{
    // Use the same LSID as the redundant version, but replace
    // 'redundant' with 'nr'.
    sprintf(zSql, "SELECT libLSID FROM %s.LibInfo", redundantDbName_);
    
    int iRow, iCol;
    char** result;
    int rc = sqlite3_get_table(getDb(), zSql, &result, &iRow, &iCol, 0);
    
    check_rc(rc, zSql);
    
    string libLSID = result[1];
    const string redundant = ":redundant:";
    size_t idx = libLSID.find(redundant);
    if (idx == string::npos)
    {
        throw BlibException(false, "The library %s does not appear to be a redundant library.\n"
                            "'%s' was not found in LSID '%s'.",
                            redundantFileName_.c_str(), redundant.c_str(), libLSID.c_str());
    }

    libLSID.replace(idx, redundant.length(), ":nr:");
    
    sqlite3_free_table(result);
    return libLSID;
}

void BlibFilter::getNextRevision(int* major, int* minor)
{
    // Use same revision as the redundant version
    sprintf(zSql, "SELECT majorVersion, minorVersion FROM %s.LibInfo", 
            redundantDbName_);
    
    int iRow, iCol;
    char** result;
    int rc = sqlite3_get_table(getDb(), zSql, &result, &iRow, &iCol, 0);
    
    check_rc(rc, zSql);  // does this also check that there is at least one row returned?
    
    *major = atoi(result[2]);
    *minor = atoi(result[3]);
    
    sqlite3_free_table(result);
}

/**
 *  Initialize a new filtered library.  First call the parent classes
 *  init method which opens a new file (taking care of overwriting) and
 *  creates standard tables.  Here add additional tables required for
 *  filtered libraries: PeptideIons and RetentionTimes.
 */
void BlibFilter::init(){
    BlibMaker::init();
    strcpy(zSql,
           "CREATE TABLE RetentionTimes (RefSpectraID INTEGER, "
           "RedundantRefSpectraID INTEGER, "
           "SpectrumSourceID INTEGER, "
           "ionMobilityValue REAL, "
           "ionMobilityType INTEGER, "
           "ionMobilityHighEnergyDriftTimeOffsetMsec REAL, "
           "retentionTime REAL, "
           "bestSpectrum INTEGER, " // boolean
           "FOREIGN KEY(RefSpectraID) REFERENCES RefSpectra(id) )" );
    sql_stmt(zSql);

}

void BlibFilter::buildNonRedundantLib()
{
    Verbosity::debug("Starting buildNonRedundant.");

    if (useBestScoring_) {
        sqlite3_stmt* scoreStmt;
        if (sqlite3_prepare(getDb(), "SELECT id, scoreType FROM ScoreTypes", -1, &scoreStmt, 0) != SQLITE_OK)
            Verbosity::error("Failed to prepare statement to get scores types");
		
		sqlite3_stmt* checkForScore;
		sprintf(zSql, "SELECT EXISTS(SELECT 1 FROM %s.RefSpectra WHERE scoreType = ?)", redundantDbName_);
		if (sqlite3_prepare(getDb(), zSql, -1, &checkForScore, 0) != SQLITE_OK)
			Verbosity::error("Failed to prepare statement to check for score type");

        while (sqlite3_step(scoreStmt) == SQLITE_ROW) {
            int scoreTypeId = sqlite3_column_int(scoreStmt, 0);
            string scoreType = (const char*)sqlite3_column_text(scoreStmt, 1);
            if (scoreType == "PERCOLATOR QVALUE" ||
                scoreType == "PEPTIDE PROPHET SOMETHING" ||
                scoreType == "SPECTRUM MILL" ||
                scoreType == "IDPICKER FDR" ||
                scoreType == "MASCOT IONS SCORE" ||
                scoreType == "TANDEM EXPECTATION VALUE" ||
                scoreType == "WATERS MSE PEPTIDE SCORE" ||
                scoreType == "OMSSA EXPECTATION SCORE" ||
                scoreType == "PROTEIN PROSPECTOR EXPECTATION SCORE" ||
                scoreType == "SEQUEST XCORR" ||
                scoreType == "MAXQUANT SCORE" ||
                scoreType == "MORPHEUS SCORE" ||
                scoreType == "MSGF+ SCORE") {
                higherIsBetter_[scoreTypeId] = false;
            } else
            if (scoreType == "PROTEIN PILOT CONFIDENCE" ||
                scoreType == "SCAFFOLD SOMETHING" ||
                scoreType == "PEAKS CONFIDENCE SCORE") {
                higherIsBetter_[scoreTypeId] = true;
            } else {
                Verbosity::warn("Don't know if higher or lower is better: %s", scoreType.c_str());
				sqlite3_bind_int(checkForScore, 1, scoreTypeId);
				if (sqlite3_step(checkForScore) != SQLITE_ROW)
					Verbosity::error("Failed to execute statement to check for score type");
				if (sqlite3_column_int(checkForScore, 0) == 1) {
					// Cannot filter by score if we don't know whether a higher/lower score is better
					// Revert to normal behavior
					Verbosity::warn("Cannot filter by score, reverting to normal behavior");
					useBestScoring_ = false;
					break;
				}
				sqlite3_reset(checkForScore);
            }
        }
        sqlite3_finalize(scoreStmt);
		sqlite3_finalize(checkForScore);
    }

    string msg = "ERROR: Failed building library ";
    msg += getLibName();
    setMessage(msg.c_str());

    // first copy over all of the spectrum source files
    transferSpectrumFiles(redundantDbName_);

    // find out if we have retention times and other additional columns
    tableVersion_ = 0;
    string optional_cols;
    if (tableColumnExists(redundantDbName_, "RefSpectra", "retentionTime")) {
        ++tableVersion_;
        optional_cols = ", SpecIDinFile, retentionTime";
        if (tableColumnExists(redundantDbName_, "RefSpectra", "ionMobilityValue")) {
            ++tableVersion_;
            optional_cols += ", ionMobilityValue, ionMobilityType";
        }
        if (tableColumnExists(redundantDbName_, "RefSpectra", "ionMobilityHighEnergyDriftTimeOffsetMsec")) {
            ++tableVersion_;
            optional_cols += ", ionMobilityHighEnergyDriftTimeOffsetMsec";
        }
    }

    vector<RefSpectrum*> oneIon;
    char lastPepModSeq[1024], pepModSeq[1024];
    lastPepModSeq[0]='\0';
    pepModSeq[0]='\0';

    int lastCharge=0, charge=0;

    Verbosity::debug("Counting Spectra.");
    ProgressIndicator progress(getSpectrumCount(redundantDbName_));

    Verbosity::debug("Sorting spectra by sequence and charge.");
    //first Order by peptideModSeq and charge, filter by num peaks
    sprintf(zSql,
            "SELECT id,peptideSeq,precursorMZ,precursorCharge,peptideModSeq,"
            "prevAA, nextAA, numPeaks, score, scoreType %s "
            "FROM %s.RefSpectra "
            "WHERE numPeaks >= %i "
            "ORDER BY peptideModSeq, precursorCharge %s", optional_cols.c_str(), redundantDbName_, minPeaks_,
            optional_cols.c_str());

    smart_stmt pStmt;
    int rc = sqlite3_prepare(getDb(), zSql, -1, &pStmt, 0);

    check_rc(rc, zSql, 
             "Failed selecting redundant spectra for comparison.");
    Verbosity::debug("Successfully sorted.");

    rc = sqlite3_step(pStmt);

    // setup for getting peak data
    sqlite3* peakConnection;
    int peakRc = sqlite3_open(redundantFileName_.c_str(), &peakConnection);
    if (peakRc != SQLITE_OK)
    {
        Verbosity::error("Could not open connection to database '%s'", redundantFileName_.c_str());
    }
    sqlite3_stmt* peakStmt;
    char zSqlPeakQuery[2048];
    strcpy(zSqlPeakQuery,
           "SELECT peakMZ, peakIntensity "
           "FROM RefSpectraPeaks "
           "WHERE RefSpectraId = ");
    char* idPos = zSqlPeakQuery;
    idPos += strlen(zSqlPeakQuery);

    // for each spectrum entry in table
    while( rc==SQLITE_ROW ) {
        progress.increment();
        
        // create a RefSpectrum object and populate all fields
        // then get charge and seq
        // TODO: RefSpectrum* tmpRef = nextRefSpec(pStmt);

        RefSpectrum* tmpRef = new RefSpectrum();

        strcpy(pepModSeq,
               reinterpret_cast<const char*>(sqlite3_column_text(pStmt,4)));
        charge = sqlite3_column_int(pStmt,3);
        
        tmpRef->setLibSpecID(sqlite3_column_int(pStmt,0));
        tmpRef->setSeq(reinterpret_cast<const char*>(sqlite3_column_text(pStmt,
                                                                         1)));
        tmpRef->setMz(sqlite3_column_double(pStmt,2));
        tmpRef->setCharge(charge);
        // if not selected, value == 0
        tmpRef->setIonMobility(sqlite3_column_double(pStmt, 12));
        tmpRef->setIonMobilityType(sqlite3_column_int(pStmt, 13));
        tmpRef->setIonMobilityHighEnergyDriftTimeOffsetMsec(sqlite3_column_double(pStmt, 14));
        tmpRef->setRetentionTime(sqlite3_column_double(pStmt, 11));
        tmpRef->setMods(pepModSeq);
        tmpRef->setPrevAA("-");
        tmpRef->setNextAA("-");
        tmpRef->setScore(sqlite3_column_double(pStmt, 8));
        tmpRef->setScoreType(sqlite3_column_int(pStmt, 9));
        tmpRef->setScanNumber(sqlite3_column_int(pStmt, 10));

        int numPeaks = sqlite3_column_int(pStmt,7);

        // get peaks for this spectrum
        int refSpectraId = sqlite3_column_int(pStmt, 0);
        sprintf(idPos, "%i", refSpectraId);
        peakRc = sqlite3_prepare(peakConnection, zSqlPeakQuery, -1, &peakStmt, NULL);
        check_rc(peakRc, zSqlPeakQuery, "Failed selecting peaks.");
        peakRc = sqlite3_step(peakStmt);
        if (peakRc != SQLITE_ROW)
        {
            Verbosity::error("Did not find peaks for spectrum %d.", refSpectraId);
        }
        int numBytes1 = sqlite3_column_bytes(peakStmt, 0);
        Byte* comprM = (Byte*)sqlite3_column_blob(peakStmt, 0);
        int numBytes2 = sqlite3_column_bytes(peakStmt, 1);
        Byte* comprI = (Byte*)sqlite3_column_blob(peakStmt, 1);        
        
        
        // is this slow for copying the peak vector? better to return a ptr?
        vector<PEAK_T> peaks = getUncompressedPeaks(numPeaks, numBytes1,
                                                    comprM, numBytes2, comprI);
        sqlite3_finalize(peakStmt);
        if (peaks.size() == 0) {
            Verbosity::error("Unable to read peaks for redundant library "
                             "spectrum %i, sequence %s, charge %i.",
                             tmpRef->getLibSpecID(), (tmpRef->getSeq()).c_str(),
                             tmpRef->getCharge());
        }
        tmpRef->setRawPeaks(peaks);
        // TODO end nextRefSpec

        // if this spec has same seq and charge, add to the collection
        if(strcmp(pepModSeq,lastPepModSeq) == 0 && lastCharge == charge) {
            oneIon.push_back(tmpRef);
        } else {// filter & start new collection for a different seq and charge
            
            if(!oneIon.empty()) {
                Verbosity::comment(V_DETAIL, "Selecting spec for %s, charge %i"
                                   " from %i spectra.", lastPepModSeq,
                                   lastCharge, oneIon.size());
                compAndInsert(oneIon);
                clearVector(oneIon);
            }
            
            oneIon.push_back(tmpRef);
            strcpy(lastPepModSeq, pepModSeq);
            lastCharge = charge;
            Verbosity::comment(V_DETAIL, "Collecting spec for %s, charge %i,",
                               pepModSeq, charge);
        }
        
        rc = sqlite3_step(pStmt);
    }// next table entry
    
    // Insert the last spectrum
    if (!oneIon.empty()) {
        progress.increment();
        Verbosity::comment(V_DETAIL, "Selecting spec for %s, charge %i"
                           " from %i spectra.", lastPepModSeq,
                           lastCharge, oneIon.size());
        compAndInsert(oneIon);
        clearVector(oneIon);
    }

    // we may have selected fewer spectra than were in the library
    // update the progress indicator
    progress.finish();
}

vector<PEAK_T> BlibFilter::getUncompressedPeaks(int& numPeaks,
                                                int& mzLen, Byte* comprM, 
                                                int& intensityLen, Byte* comprI)
{
    //variables for compressed files
    uLong uncomprLen;
    double *mz;
    float *intensity;

    uncomprLen=numPeaks*sizeof(double);
    if ((int)uncomprLen == mzLen)
        mz = (double*) comprM;
    else {
        mz = new double[numPeaks];
        uncompress((Bytef*)mz, &uncomprLen, comprM, mzLen);
    }

    uncomprLen=numPeaks*sizeof(float);
    if ((int)uncomprLen == intensityLen)
        intensity = (float*) comprI;
    else {
        intensity = new float[numPeaks];
        uncompress((Bytef*)intensity, &uncomprLen, comprI, intensityLen);
    }
    
    vector<PEAK_T> peaks(numPeaks);
    PEAK_T p;
    for(int i=0;i<numPeaks;i++) {
        p.mz = mz[i];
        p.intensity = intensity[i];
        // Check for corruption
        if (p.mz != p.mz || p.mz < 0 || p.mz > 100000 ||
            p.intensity != p.intensity || p.intensity < 0) {
            // Corrupted peaks
            peaks.clear();
            return peaks;
        }
        peaks[i] = p;
    }

    if (mz != (double*)comprM)
        delete [] mz;
    if (intensity != (float*)comprI)
        delete [] intensity;

    return peaks;
}

/**
 * Given a vector containing RefSpectrum for the same sequence and
 * charge, find the best representative and insert into the current table.
 * The "best representative" is currently defined as the spectrum that
 * has the highest average dot product when compared to all other
 * spectra.  
 *
 * When the collection contains exactly one spectrum, add it.  When the
 * spectrum contains exactly two spectra, the average dot product will
 * be the same for both so use a different criterion to choose.
 * Eventually, when a quailty-of-match score (e.g. p-value) is stored,
 * use the spec with the higher score.  For now, use the one with more
 * peaks. 
 */
void BlibFilter::compAndInsert(vector<RefSpectrum*>& oneIon)
{
    int num_spec = oneIon.size();
    int specID = 0; // id RefSpec table in filtered index
    int bestIndex = 0;

    if(num_spec == 1){ // add that one spectrum
        specID = transferSpectrum(redundantDbName_, 
                                  oneIon.at(0)->getLibSpecID(), 
                                  num_spec,
                                  tableVersion_);
    } else if(!useBestScoring_) { // choose the one with more peaks
		if (num_spec == 2){
			// in the future, pick the one with the best search score
			if( oneIon.at(0)->getNumRawPeaks() < oneIon.at(1)->getNumRawPeaks() ) {
				bestIndex = 1;
			}
			// cerr << "selecting index " << bestIndex << ", scan " << oneIon.at(bestIndex)->getScanNumber() << " from " << oneIon.at(0)->getScanNumber() << " and " << oneIon.at(1)->getScanNumber() << endl;

			specID = transferSpectrum(redundantDbName_, 
									  oneIon.at(bestIndex)->getLibSpecID(), 
									  num_spec,
									  tableVersion_);
		} else { // compute all-by-all dot-products

				// preprocess all RefSpectrum in oneIon
				PeakProcessor proc;
				proc.setClearPrecursor(true);
				proc.setNumTopPeaksToUse(100);
				// TODO (BF Aug-12-09): all processing should be controlled by
				// parameters available to the user
            
				RefSpectrum* tmpRef;
				for(int i=0; i<(int)oneIon.size(); i++) {
					tmpRef=oneIon.at(i);
					proc.processPeaks(tmpRef);
				}
            
				// create an array where we'll sum scores for each spectrum
				// initialize to 0
				vector<double> scores(oneIon.size(), 0);

				// for each spectrum
				for(int i=0; i<(int)oneIon.size(); i++) {
					RefSpectrum* tmpRef1 = oneIon.at(i);

					// compare to all subsequent spectrum
					for(int j=i+1; j<(int)oneIon.size(); j++) {

						RefSpectrum* tmpRef2 = oneIon.at(j);
						Match thisMatch(tmpRef1, tmpRef2);
						DotProduct::compare(thisMatch);
						double dotProduct = thisMatch.getScore(DOTP);

						// add the score to the running total for both spec
						scores[i] += dotProduct;
						scores[j] += dotProduct;
					}
				} // next spectrum

				// find the best score and keep the spectrum associated with it
				bestIndex = getMaxElementIndex(scores);
				double bestScore = scores[bestIndex];
				double bestAverageScore = bestScore / (double)oneIon.size() ;

				// If best average score is too low, don't include it 
				if ( bestAverageScore >= minAverageScore_ ){
					specID = transferSpectrum(redundantDbName_, 
											  oneIon.at(bestIndex)->getLibSpecID(), 
											  oneIon.size(),
											  tableVersion_);
				} else {
					Verbosity::warn("Best score is %f for %s, charge %d after "
									"comparing %i spectra.  This sequence will not be "
									"included in the filtered library.", 
									bestAverageScore, (oneIon.at(0)->getSeq()).c_str(),
									oneIon.at(0)->getCharge(), oneIon.size());
					return;
				}
		}
    } else {
		map<RefSpectrum*, int> indices;
        map< int, vector<RefSpectrum*> > groups = groupByScoreType(oneIon, &indices);
        RefSpectrum* winner = NULL;

        vector<RefSpectrum*> possibleWinners;
        for (map< int, vector<RefSpectrum*> >::const_iterator i = groups.begin(); i != groups.end(); ++i) {
            map<int, bool>::const_iterator directionLookup = higherIsBetter_.find(i->first);
            if (directionLookup == higherIsBetter_.end()) {
                Verbosity::error("Don't know if higher or lower is better for score type %d", i->first);
            }
            vector<RefSpectrum*> bestScores = getBestScores(i->second, directionLookup->second);
            possibleWinners.insert(possibleWinners.end(), bestScores.begin(), bestScores.end());
        }

        if (possibleWinners.size() == 1) {
            winner = possibleWinners.front();
        } else {
            // find highest TIC to determine final winner
			double winningValue = -1.0;
			for (vector<RefSpectrum*>::iterator i = possibleWinners.begin(); i != possibleWinners.end(); ++i) {
                double specValue = (*i)->getTotalIonCurrentRaw();
                if (specValue > winningValue) {
                    winner = *i;
					winningValue = specValue;
				}
			}

			/* cross-cross score among possible winners to determine final winner
			PeakProcessor proc;
            proc.setClearPrecursor(true);
            proc.setNumTopPeaksToUse(100);
            for (vector<RefSpectrum*>::iterator i = oneIon.begin(); i != oneIon.end(); ++i)
                proc.processPeaks(*i);
			
			double winningScore = -1.0;
			for (vector<RefSpectrum*>::iterator i = possibleWinners.begin(); i != possibleWinners.end(); ++i) {
                double crossScore = 0.0;
                for (vector<RefSpectrum*>::iterator j = possibleWinners.begin(); j != possibleWinners.end(); ++j) {
                    if (*i == *j)
                        continue;
                    Match thisMatch(*i, *j);
                    DotProduct::compare(thisMatch);
					crossScore += thisMatch.getScore(DOTP);
                }
                if (crossScore > winningScore) {
                    winner = *i;
                    winningScore = crossScore;
                }
			}*/
        }
        specID = transferSpectrum(redundantDbName_, winner->getLibSpecID(),
                                    oneIon.size(), tableVersion_);
		bestIndex = indices[winner];
    }

    // add rt, RefSpectraId for all refspec
    for(int i = 0; i < num_spec; i++){
        // if( oneIon.at(i)->getRetentionTime() == 0){ continue; }
        int specIdRedundant = oneIon.at(i)->getLibSpecID();
        sprintf(zSql,
                "INSERT INTO RetentionTimes (RefSpectraID, RedundantRefSpectraID, "
                "SpectrumSourceID, ionMobilityValue, ionMobilityType, ionMobilityHighEnergyDriftTimeOffsetMsec, "
                "retentionTime, bestSpectrum) "
                "VALUES (%d, %d, %d, %f, %d, %f, %f, %d)",
                specID,
                specIdRedundant,
                getNewFileId(redundantDbName_, specIdRedundant),  // All files should exist by now
                oneIon.at(i)->getIonMobility(),
                oneIon.at(i)->getIonMobilityType(),
                oneIon.at(i)->getIonMobilityHighEnergyDriftTimeOffsetMsec(),
                oneIon.at(i)->getRetentionTime(),
                i == bestIndex ? 1 : 0);
        sql_stmt(zSql);
    }
}

map< int, vector<RefSpectrum*> > BlibFilter::groupByScoreType(const vector<RefSpectrum*>& oneIon, map<RefSpectrum*, int>* outIndices) {
    map< int, vector<RefSpectrum*> > groups;
	int index = -1;
    for (vector<RefSpectrum*>::const_iterator i = oneIon.begin(); i != oneIon.end(); ++i) {
        int scoreType = (*i)->getScoreType();
        map< int, vector<RefSpectrum*> >::iterator lookup = groups.find(scoreType);
        if (lookup == groups.end())
            groups[scoreType] = vector<RefSpectrum*>(1, *i);
        else
            groups[scoreType].push_back(*i);
		if (outIndices)
			(*outIndices)[*i] = ++index;
    }
    return groups;
}

vector<RefSpectrum*> BlibFilter::getBestScores(const vector<RefSpectrum*>& group, bool higherIsBetter) {
    vector<RefSpectrum*> best;
    if (group.empty())
        return best;

    best.push_back(group.front());
    if (group.size() == 1)
        return best;

    double bestScore = best.front()->getScore();
    
    for (vector<RefSpectrum*>::const_iterator i = group.begin() + 1; i != group.end(); ++i) {
        double score = (*i)->getScore();
        if ((!higherIsBetter && score < bestScore) ||
            (higherIsBetter && score > bestScore)) {
            best.clear();
            best.push_back(*i);
            bestScore = score;
        } else if (score == bestScore) {
            best.push_back(*i);
        }
    }
    return best;
}


/*
 * Local Variables:
 * mode: c
 * c-basic-offset: 4
 * End:
 */
