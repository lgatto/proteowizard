// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently,
// but are changed infrequently

#pragma once

#include <stdlib.h>
#include <vcclr.h>
#include <string>

bool ToCharStar( System::String^ source, char*& target );
bool ToStdString( System::String^ source, std::string &target );
std::string ToStdString( System::String^ source );