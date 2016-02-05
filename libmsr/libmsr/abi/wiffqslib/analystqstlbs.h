// -*- mode: c++ -*-


/*
    File: analysttlbs.h
    Description: AnalystQS typelibs information via #import directives
                 macro _IMPORT_PROCESSING_ is meant for generating 
                 .tlh and .tli to be included in mzWiff
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


#ifdef _IMPORT_PROCESSING_
#import "..\\libs\\analystqs\\MSMethodSvr.dll" rename_namespace("AnalystQS") exclude("IStorage","IStream","ISequentialStream","_LARGE_INTEGER","_ULARGE_INTEGER","tagSTATSTG","_FILETIME","wireSNB","tagRemSNB","IEnumSTATSTG","IPersistStorage","IPersist","ISupportErrorInfo")
#else
#include "msmethodsvr.tlh"
//#include "wiffqslib\\msmethodsvr.tli"
#endif

#ifdef _IMPORT_PROCESSING_
#import "..\\libs\\analystqs\\ExploreDataObjects.dll" rename_namespace("AnalystQS") exclude("ISequentialStream")
#else
#include "ExploreDataObjects.tlh"
//#include "wiffqslib\\ExploreDataObjects.tli"
#endif

#ifdef _IMPORT_PROCESSING_
#import "..\\libs\\analystqs\\WIFFTransSvr.dll" rename_namespace("AnalystQS") exclude("IStorage","IStream","ISequentialStream","_LARGE_INTEGER","_ULARGE_INTEGER","tagSTATSTG","_FILETIME","wireSNB","tagRemSNB","IEnumSTATSTG")
#else
#include "WIFFTransSvr.tlh"
//#include "wiffqslib\\WIFFTransSvr.tli"
#endif

#ifdef _IMPORT_PROCESSING_
#import "..\\libs\\analystqs\\Peak_Finder2.dll" rename_namespace("AnalystQS") 
#else
#include "Peak_Finder2.tlh"
//#include "wiffqslib\\Peak_Finder2.tli"
#endif

#ifdef _IMPORT_PROCESSING_
#import "..\\libs\\analystqs\\PeakFinderFactory.dll" rename_namespace("AnalystQS") 
#else
#include "PeakFinderFactory.tlh"
//#include "wiffqslib\\PeakFinderFactory.tli"
#endif

#ifdef _IMPORT_PROCESSING_
#import "..\\libs\\analystqs\\FileManager.dll" rename_namespace("AnalystQS") exclude("IStorage","IStream","ISequentialStream","_LARGE_INTEGER","_ULARGE_INTEGER","tagSTATSTG","_FILETIME","wireSNB","tagRemSNB","IEnumSTATSTG")
#else
#include "FileManager.tlh"
//#include "wiffqslib\\FileManager.tli"
#endif

#ifdef _IMPORT_PROCESSING_
#import "..\\libs\\analystqs\\AcqMethodSvr.dll" rename_namespace("AnalystQS") exclude("tagACQMETHODSTGTYPE", "IPersistWIFFStg", "IEnableTuneMode","IStorage","IStream","ISequentialStream","_LARGE_INTEGER","_ULARGE_INTEGER","tagSTATSTG","_FILETIME","wireSNB","tagRemSNB","IEnumSTATSTG","IPersistStorage","IPersist","ISupportErrorInfo")
#else
#include "AcqMethodSvr.tlh"
//#include "wiffqslib\\AcqMethodSvr.tli"
#endif

#ifdef _IMPORT_PROCESSING_
#import "..\\libs\\analystqs\\ParameterSvr.dll" rename_namespace("AnalystQS") exclude("IStorage","IStream","ISequentialStream","_LARGE_INTEGER","_ULARGE_INTEGER","tagSTATSTG","_FILETIME","wireSNB","tagRemSNB","IEnumSTATSTG","IPersistStorage","IPersist","ISupportErrorInfo","tagACQMETHODSTGTYPE","IPersistWIFFStg","IMiscDAMCalls")
#else
#include "ParameterSvr.tlh"
//#include "wiffqslib\\ParameterSvr.tli"
#endif
