#ifndef _INCLUDED_COMMON_H_
#define _INCLUDED_COMMON_H_

#define BOOST_LIB_DIAGNOSTIC

#ifdef LIBMSR_DYN_LINK
#ifdef LIBMSR_SOURCE
#define LIBMSR_DECL __declspec(dllexport)
#else
#define LIBMSR_DECL __declspec(dllimport)
#endif  // LIBMSR_SOURCE
#endif  // DYN_LINK

// if LIBMSR_DECL isn't defined yet define it now:
#ifndef LIBMSR_DECL
#define LIBMSR_DECL
#endif

#include "platform.h"

#include <iostream>
#include <fstream>
#include <string>
#include <memory>
#include <exception>
#include <stdexcept>
#include <typeinfo>
#include <vector>
#include <map>
#include <list>
#include <set>
#include <stack>
#include <algorithm>
#include <cstdio>
#include <cmath>
#include <limits>

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/cstdint.hpp>
#include <boost/foreach.hpp>

using std::auto_ptr;

using std::cout;
using std::cerr;
using std::endl;
using std::flush;

//using std::string;
/*#ifdef _UNICODE
#define ios wios
#define fstream wfstream
#define ostream wostream
#define ofstream wofstream
#define istream wistream
#define ifstream wifstream
#define string wstring
#define stringstream wstringstream
using std::wiostream;
using std::wfstream;
using std::wostream;
using std::wofstream;
using std::wistream;
using std::wifstream;
using std::wstring;
using std::wstringstream;
#define CHAR_ARRAY(str) L##str
#else*/
using std::ios;
using std::iostream;
using std::fstream;
using std::ostream;
using std::ofstream;
using std::istream;
using std::ifstream;
using std::string;
using std::stringstream;
//#define CHAR_ARRAY(str) str
//#endif

using std::streamsize;

using std::map;
using std::multimap;
using std::vector;
//using std::deque;
using std::set;
using std::multiset;
using std::list;
using std::pair;

using std::remove;
using std::remove_if;
using std::copy;
//using std::accumulate;

typedef std::ostream_iterator<char> ostream_inserter;
typedef std::istream_iterator<char> istream_inserter;
using std::back_inserter;
using std::front_inserter;
using std::advance;

using std::internal;
using std::right;
using std::left;
using std::boolalpha;
using std::noboolalpha;
using std::showpoint;
using std::noshowpoint;
using std::showpos;
using std::noshowpos;
using std::fixed;
using std::dec;
using std::scientific;
using std::min;
using std::max;

using std::exception;
using std::invalid_argument;
using std::out_of_range;
using std::overflow_error;
using std::runtime_error;

using boost::lexical_cast;
using boost::algorithm::to_lower;
using boost::algorithm::to_upper;
using boost::algorithm::to_lower_copy;
using boost::algorithm::to_upper_copy;
using boost::algorithm::split;


#define SAFE_DELETE(p) if(p) { delete p; p = NULL; }
#define SAFE_DELETE_ARRAY(p) if(p) { delete [] p; p = NULL; }

enum LIBMSR_DECL HostEndianType { COMMON_LITTLE_ENDIAN, COMMON_BIG_ENDIAN, COMMON_UNKNOWN_ENDIAN };

string LIBMSR_DECL GetPathnameFromFilepath( const string& filepath );
string LIBMSR_DECL GetFilenameExtension( const string& filename );
void LIBMSR_DECL FindFilesByMask( const string& mask, set<string>& filenames, size_t maxFileCount = std::numeric_limits<size_t>::max() );
HostEndianType LIBMSR_DECL GetHostEndianType();

// operator<< for std containers
/*namespace std
{
	template< class T1, class T2 >
	ostream&		operator<< ( ostream& o, const pair< T1, T2 >& p )
	{
		return ( o << "( " << p.first << ", " << p.second << " )" );
	}

	template< class T >
	ostream&		operator<< ( ostream& o, const vector< T >& v )
	{
		o << "(";
		for( typename vector< T >::const_iterator itr = v.begin(); itr != v.end(); ++itr )
			o << " " << *itr;
		o << " )";

		return o;
	}

	template< class T, class P >
	ostream&		operator<< ( ostream& o, const set< T, P >& s )
	{
		o << "(";
		for( typename set< T, P >::const_iterator itr = s.begin(); itr != s.end(); ++itr )
			o << " " << *itr;
		o << " )";

		return o;
	}

	inline ostream&		operator<< ( ostream& o, const map< string, string >& m )
	{
		o << "(";
		for( map< string, string >::const_iterator itr = m.begin(); itr != m.end(); ++itr )
			o << " \"" << itr->first << "\"->\"" << itr->second << "\"";
		o << " )";

		return o;
	}

	template< class KeyT >
	ostream&		operator<< ( ostream& o, const map< KeyT, string >& m )
	{
		o << "(";
		for( typename map< KeyT, string >::const_iterator itr = m.begin(); itr != m.end(); ++itr )
			o << " " << itr->first << "->\"" << itr->second << "\"";
		o << " )";

		return o;
	}

	template< class ValueT >
	ostream&		operator<< ( ostream& o, const map< string, ValueT >& m )
	{
		o << "(";
		for( typename map< string, ValueT >::const_iterator itr = m.begin(); itr != m.end(); ++itr )
			o << " \"" << itr->first << "\"->" << itr->second << "";
		o << " )";

		return o;
	}

	template< class KeyT, class ValueT >
	ostream&		operator<< ( ostream& o, const map< KeyT, ValueT >& m )
	{
		o << "(";
		for( typename map< KeyT, ValueT >::const_iterator itr = m.begin(); itr != m.end(); ++itr )
			o << " " << itr->first << "->" << itr->second << "";
		o << " )";

		return o;
	}

	template< class KeyT, class ValueT, class PredT >
	vector< KeyT > keys( const map< KeyT, ValueT, PredT >& m )
	{
		vector< KeyT > keys;
		keys.reserve( m.size() );
		for( typename map< KeyT, ValueT, PredT >::const_iterator itr = m.begin(); itr != m.end(); ++itr )
			keys.push_back( itr->first );
		return keys;
	}

	template< class KeyT, class ValueT, class PredT >
	vector< ValueT > values( const map< KeyT, ValueT, PredT >& m )
	{
		vector< ValueT > values;
		values.reserve( m.size() );
		for( typename map< KeyT, ValueT, PredT >::const_iterator itr = m.begin(); itr != m.end(); ++itr )
			values.push_back( itr->second );
		return values;
	}

	template< class ValueT >
	typename map< string, ValueT >::const_iterator find_first_of( const map< string, ValueT >& m, const string& keyList, const string& keyListDelimiters = " " )
	{
		vector< string > keys;
		split( keys, keyList, boost::is_any_of( keyListDelimiters ) );
		typename map< string, ValueT >::const_iterator itr = m.end();
		for( size_t i=0; i < keys.size() && itr == m.end(); ++i )
			itr = m.find( keys[i] );
		return itr;
	}

	template< class ValueT >
	typename map< string, ValueT >::iterator find_first_of( map< string, ValueT >& m, const string& keyList, const string& keyListDelimiters = " " )
	{
		vector< string > keys;
		split( keys, keyList, boost::is_any_of( keyListDelimiters ) );
		typename map< string, ValueT >::iterator itr = m.end();
		for( size_t i=0; i < keys.size() && itr == m.end(); ++i )
			itr = m.find( keys[i] );
		return itr;
	}

	template< class KeyT, class ValueT, class PredT >
	typename map< KeyT, ValueT, PredT >::const_iterator find_first_of( const map< KeyT, ValueT, PredT >& m, const string& keyList, const string& keyListDelimiters = " " )
	{
		vector< string > keyStrings;
		split( keyStrings, keyList, boost::is_any_of( keyListDelimiters ) );
		vector< KeyT > keys;
		std::transform( keyStrings.begin(), keyStrings.end(), keys.begin(), lexical_cast< KeyT, string >() );
		typename map< KeyT, ValueT, PredT >::const_iterator itr = m.end();
		for( size_t i=0; i < keys.size() && itr == m.end(); ++i )
			itr = m.find( keys[i] );
		return itr;
	}

	template< class KeyT, class ValueT, class PredT >
	typename map< KeyT, ValueT, PredT >::iterator find_first_of( map< KeyT, ValueT, PredT >& m, const string& keyList, const string& keyListDelimiters = " " )
	{
		vector< string > keyStrings;
		split( keyStrings, keyList, boost::is_any_of( keyListDelimiters ) );
		vector< KeyT > keys;
		std::transform( keyStrings.begin(), keyStrings.end(), keys.begin(), lexical_cast< KeyT, string >() );
		typename map< KeyT, ValueT, PredT >::iterator itr = m.end();
		for( size_t i=0; i < keys.size() && itr == m.end(); ++i )
			itr = m.find( keys[i] );
		return itr;
	}
}

// optimized lexical_cast specializations
namespace boost
{
	template<>
	inline float lexical_cast( const std::string& str )
	{
		return (float) strtod( str.c_str(), NULL );
	}

	template<>
	inline double lexical_cast( const std::string& str )
	{
		return strtod( str.c_str(), NULL );
	}

	template<>
	inline int lexical_cast( const std::string& str )
	{
		return atoi( str.c_str() );
	}

	template<>
	inline long lexical_cast( const std::string& str )
	{
		return atol( str.c_str() );
	}

	template<>
	inline unsigned int lexical_cast( const std::string& str )
	{
		return (unsigned int) atoi( str.c_str() );
	}

	template<>
	inline unsigned long lexical_cast( const std::string& str )
	{
		return (unsigned long) atol( str.c_str() );
	}
}*/

#endif
