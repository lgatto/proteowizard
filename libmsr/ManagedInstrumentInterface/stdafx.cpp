// stdafx.cpp : source file that includes just the standard includes
// ManagedInstrumentInterface.pch will be the pre-compiled header
// stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"

bool ToCharStar( System::String^ source, char*& target )
{
	int len = (( source->Length+1) * 2);
	target = new char[ len ];
	pin_ptr<const wchar_t> wch = PtrToStringChars( source );
	return wcstombs( target, wch, len ) != -1;
}

bool ToStdString( System::String^ source, std::string &target )
{
	int len = (( source->Length+1) * 2);
	char *ch = new char[ len ];
	bool result ;
	{
		pin_ptr<const wchar_t> wch = PtrToStringChars( source );
		result = wcstombs( ch, wch, len ) != -1;
	}
	target = ch;
	delete ch;
	return result;
}

std::string ToStdString( System::String^ source )
{
	int len = (( source->Length+1) * 2);
	char *ch = new char[ len ];
	bool result ;
	{
		pin_ptr<const wchar_t> wch = PtrToStringChars( source );
		result = wcstombs( ch, wch, len ) != -1;
	}
	std::string target = ch;
	delete ch;
	if(!result)
		throw std::runtime_error("error converting System::String to std::string");
	return target;
}
