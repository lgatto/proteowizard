#define LIBMSR_SOURCE
#include "common.h"

LIBMSR_DECL string GetPathnameFromFilepath( const string& filepath )
{
	return filepath.substr( 0, filepath.find_last_of( SYS_PATH_DELIMITER ) + 1 );
}

LIBMSR_DECL string GetFilenameExtension( const string& filename )
{
	size_t extDot = filename.find_last_of( '.' );
	if( extDot != string::npos )
		return filename.substr( extDot+1 );
	else
		return "";
}

LIBMSR_DECL void FindFilesByMask( const string& mask, set<string>& filenames, size_t maxFileCount )
{
#ifdef WIN32
	string maskPathname = GetPathnameFromFilepath( mask );
	WIN32_FIND_DATA fdata;
	HANDLE srcFile = FindFirstFileEx( mask.c_str(), FindExInfoStandard, &fdata, FindExSearchNameMatch, NULL, 0 );
	if( srcFile == INVALID_HANDLE_VALUE )
		return;

	do
	{
		filenames.insert( maskPathname + fdata.cFileName );
	} while( filenames.size() < maxFileCount && FindNextFile( srcFile, &fdata ) );

	FindClose( srcFile );

#else

	glob_t globbuf;
	int rv = glob( mask.c_str(), 0, NULL, &globbuf );
	if( rv > 0 && rv != GLOB_NOMATCH )
		throw runtime_error( "FindFilesByMask(): glob() error" );

	DIR* curDir = opendir( "." );
	struct stat curEntryData;

	for( size_t i=0; filenames.size() < maxFileCount && i < globbuf.gl_pathc; ++i )
	{
		stat( globbuf.gl_pathv[i], &curEntryData );
		if( S_ISREG( curEntryData.st_mode ) )
			filenames.insert( globbuf.gl_pathv[i] );
	}
	closedir( curDir );

	globfree( &globbuf );

#endif
}

LIBMSR_DECL HostEndianType GetHostEndianType()
{
	int testInt = 127;
	char* testIntP = (char*) &testInt;

	if( testIntP[0] == 127 )
		return COMMON_LITTLE_ENDIAN;
	else if( testIntP[ sizeof(int)-1 ] == 127 )
		return COMMON_BIG_ENDIAN;
	else
		return COMMON_UNKNOWN_ENDIAN;
}
