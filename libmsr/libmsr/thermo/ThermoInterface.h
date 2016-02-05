// -*- mode: c++ -*-


/*
    File: ThermoInterface.h
    Description: Encapsulation for Thermo Xcalibur interface.
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


#ifndef _INCLUDED_THERMOINTERFACE_H_
#define _INCLUDED_THERMOINTERFACE_H_

#include "common.h"
#include "xrawfile2.h"
#include "BaseInstrumentInterface.h"
#include "FilterLine.h"


typedef struct _datapeak
{
	double dMass;
	double dIntensity;
} DataPeak;


class ThermoInterface : public BaseInstrumentInterface {
private:
	// COleDispatchDriver object instance to xrawfile2 dll
	IXRawfile2 xrawfile2_;
	int msControllerType_;
	bool firstTime_;
	bool preferVendorCentroid_;
	void getPrecursorInfo(Scan& scan, FilterLine& filterLine);

public:
	ThermoInterface();
	~ThermoInterface();

	virtual bool initInterface();
	virtual bool setInputFile(const string& fileName, const ProcessingOptions& options);
	virtual void setCentroiding(bool centroid, bool tandemCentroid, bool preferVendor);
	virtual void setDeisotoping(bool deisotope);
	virtual void setVerbose(bool verbose);

	virtual Scan* getScan()							{ return getScan(0, true); }
	virtual Scan* getScanHeader()					{ return getScan(0, false); }

	virtual Scan* getScan(long scanNumber)			{ return getScan(scanNumber, true); }
	virtual Scan* getScanHeader(long scanNumber)	{ return getScan(scanNumber, false); }

private:
	virtual Scan* getScan(long scanNumber, bool getPeaks);
};

#endif
