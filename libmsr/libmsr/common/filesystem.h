#ifndef _INCLUDED_FILESYSTEM_H_
#define _INCLUDED_FILESYSTEM_H_

#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/convenience.hpp>

#ifdef WIN32
#define MAKE_PATH_FOR_BOOST(str) (boost::algorithm::replace_all_copy( str, "\\", "/" ))
#else
#define MAKE_PATH_FOR_BOOST(str) (str)
#endif

namespace filesystem = boost::filesystem;
using filesystem::path;
using filesystem::file_size;
using filesystem::last_write_time;
using filesystem::exists;
using filesystem::current_path;
using filesystem::change_extension;
using filesystem::basename;
using filesystem::extension;
using filesystem::directory_iterator;

#endif
