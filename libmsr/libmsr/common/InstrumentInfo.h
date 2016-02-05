// -*- mode: c++ -*-


/*
	File: InstrumentInfo.h
	Description: simple struct to hold machine info.
	Date: July 25, 2007

	Copyright (C) 2007 Joshua Tasman, ISB Seattle


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



#ifndef _INCLUDED_INSTRUMENTINFO_H_
#define _INCLUDED_INSTRUMENTINFO_H_

#include "MSTypes.h"

class InstrumentInfo {
public:
	MSManufacturerType manufacturer_; // Waters, etc
	MSInstrumentModelType instrumentModel_; // LTQ Deca, etc
	MSIonizationType ionSource_; // esi, nsi, etc

	// this is a list, in case of dual mode machines like FT/IT
	std::vector<MSAnalyzerType> analyzerList_; // ion trap, etc

	MSDetectorType detector_; // electron multiplier, etc
	string instrumentName_; // "Machine #7", etc
	string instrumentSerialNumber_;
	string instrumentHardwareVersion_;

	MSAcquisitionSoftwareType acquisitionSoftware_; // xcalibur, masslynx, masshunter, etc
	string acquisitionSoftwareVersion_;

	CVAccession cvManufacturer_;
	CVAccession cvModel_;
	CVAccession cvIonSource_;
	std::vector< CVAccession > cvAnalyzerList_;
	CVAccession cvDetector_;

	CVAccession cvAcquisitionSoftware_;
	//CVAccession cvAcquisitionSoftwareVersion_;

	InstrumentInfo(void) : manufacturer_(MANUFACTURER_UNDEF),
		instrumentModel_(INSTRUMENTMODEL_UNDEF),
		ionSource_(IONIZATION_UNDEF),
		detector_(DETECTOR_UNDEF),
		acquisitionSoftware_(ACQUISITIONSOFTWARE_UNDEF)
	{
	}
};

#endif //_INCLUDED_INSTRUMENTINFO_H_
