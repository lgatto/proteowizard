// -*- mode: c++ -*-


/*
    File: analysttlbs.h
    Description: Analyst typelibs information via #import directives
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
#import "..\\libs\\analyst\\MSMethodSvr.dll" rename_namespace("Analyst") exclude("IStorage","IStream","ISequentialStream","_LARGE_INTEGER","_ULARGE_INTEGER","tagSTATSTG","_FILETIME","wireSNB","tagRemSNB","IEnumSTATSTG","IPersistStorage","IPersist","ISupportErrorInfo")
#else
#include "wifflib\\msmethodsvr.tlh"
//#include "wifflib\\msmethodsvr.tli"
#endif

#ifdef _IMPORT_PROCESSING_
#import "..\\libs\\analyst\\ExploreDataObjects.dll" rename_namespace("Analyst") exclude("ISequentialStream")
#else
#include "wifflib\\ExploreDataObjects.tlh"
//#include "wifflib\\ExploreDataObjects.tli"
#endif

#ifdef _IMPORT_PROCESSING_
#import "..\\libs\\analyst\\WIFFTransSvr.dll" rename_namespace("Analyst") exclude("IStorage","IStream","ISequentialStream","_LARGE_INTEGER","_ULARGE_INTEGER","tagSTATSTG","_FILETIME","wireSNB","tagRemSNB","IEnumSTATSTG")
#else
#include "wifflib\\WIFFTransSvr.tlh"
//#include "wifflib\\WIFFTransSvr.tli"
#endif

// CParamSettings.h
//#import "..\\libs\\analyst\\ParamSettings.dll" rename_namespace("Analyst") exclude("IPersistStorage","IPersist","IStorage","IStream","ISequentialStream","_LARGE_INTEGER","_ULARGE_INTEGER","tagSTATSTG","_FILETIME","wireSNB","tagRemSNB","IEnumSTATSTG")

#ifdef _IMPORT_PROCESSING_
#import "..\\libs\\analyst\\Peak_Finder2.dll" rename_namespace("Analyst") 
#else
#include "wifflib\\Peak_Finder2.tlh"
//#include "wifflib\\Peak_Finder2.tli"
#endif

#ifdef _IMPORT_PROCESSING_
#import "..\\libs\\analyst\\PeakFinderFactory.dll" rename_namespace("Analyst") 
#else
#include "wifflib\\PeakFinderFactory.tlh"
//#include "wifflib\\PeakFinderFactory.tli"
#endif

#ifdef _IMPORT_PROCESSING_
#import "..\\libs\\analyst\\FileManager.dll" rename_namespace("Analyst") exclude("IStorage","IStream","ISequentialStream","_LARGE_INTEGER","_ULARGE_INTEGER","tagSTATSTG","_FILETIME","wireSNB","tagRemSNB","IEnumSTATSTG")
#else
#include "wifflib\\FileManager.tlh"
//#include "wifflib\\FileManager.tli"
#endif

#ifdef _IMPORT_PROCESSING_
#import "..\\libs\\analyst\\AcqMethodSvr.dll" rename_namespace("Analyst") exclude("tagACQMETHODSTGTYPE", "IPersistWIFFStg", "IEnableTuneMode","IStorage","IStream","ISequentialStream","_LARGE_INTEGER","_ULARGE_INTEGER","tagSTATSTG","_FILETIME","wireSNB","tagRemSNB","IEnumSTATSTG","IPersistStorage","IPersist","ISupportErrorInfo")
#else
#include "wifflib\\AcqMethodSvr.tlh"
//#include "wifflib\\AcqMethodSvr.tli"
#endif

#ifdef _IMPORT_PROCESSING_
#import "..\\libs\\analyst\\ParameterSvr.dll" rename_namespace("Analyst") exclude("IStorage","IStream","ISequentialStream","_LARGE_INTEGER","_ULARGE_INTEGER","tagSTATSTG","_FILETIME","wireSNB","tagRemSNB","IEnumSTATSTG","IPersistStorage","IPersist","ISupportErrorInfo","tagACQMETHODSTGTYPE","IPersistWIFFStg","IMiscDAMCalls")
#else
#include "wifflib\\ParameterSvr.tlh"
//#include "wifflib\\ParameterSvr.tli"
#endif
