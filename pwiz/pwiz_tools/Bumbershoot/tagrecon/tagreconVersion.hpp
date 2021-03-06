//
// $Id: tagreconVersion.hpp 8278 2011-07-21 17:32:40Z chambm $
//

#ifndef _TAGRECON_VERSION_HPP_
#define _TAGRECON_VERSION_HPP_

#include <string>

namespace freicore {
namespace tagrecon {

struct Version
{
    static int Major();
    static int Minor();
    static int Revision();
    static std::string str();
    static std::string LastModified();
};

} // namespace tagrecon
} // namespace freicore

#endif // _TAGRECON_VERSION_HPP_
