//
// $Id: quameterVersion.hpp 8415 2011-09-29 21:07:27Z chambm $
//

#ifndef _QUAMETER_VERSION_HPP_
#define _QUAMETER_VERSION_HPP_

#include <string>

namespace freicore {
namespace quameter {

struct Version
{
    static int Major();
    static int Minor();
    static int Revision();
    static std::string str();
    static std::string LastModified();
};

} // namespace quameter
} // namespace freicore

#endif // _QUAMETER_VERSION_HPP_