#include "BaseInstrumentInterface.h"

void BaseInstrumentInterface::doCentroidScan(Scan* scan)
{
	if(scan->isCentroided_)
	{
		cerr << "Warning: doCentroidScan called on scan that is already centroided" << endl;
		return;
	}
}
