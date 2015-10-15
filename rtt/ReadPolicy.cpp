/***************************************************************************
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU General Public                   *
 *   License as published by the Free Software Foundation;                 *
 *   version 2 of the License.                                             *
 *                                                                         *
 *   As a special exception, you may use this file as part of a free       *
 *   software library without restriction.  Specifically, if other files   *
 *   instantiate templates or use macros or inline functions from this     *
 *   file, or you compile this file and link it with other files to        *
 *   produce an executable, this file does not by itself cause the         *
 *   resulting executable to be covered by the GNU General Public          *
 *   License.  This exception does not however invalidate any other        *
 *   reasons why the executable file might be covered by the GNU General   *
 *   Public License.                                                       *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU General Public             *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 59 Temple Place,                                    *
 *   Suite 330, Boston, MA  02111-1307  USA                                *
 *                                                                         *
 ***************************************************************************/

#include "ReadPolicy.hpp"
#include <iostream>

namespace RTT {

std::ostream &operator<<(std::ostream &os, const ReadPolicy &rp)
{
    switch(rp) {
        case UnspecifiedReadPolicy: os << "(unspecified read policy)"; break;
        case ReadShared:            os << "ReadShared"; break;
        case ReadUnordered:         os << "ReadUnordered"; break;
        default:                    os << "(unknown read policy)"; break;
    }
    return os;
}

std::istream &operator>>(std::istream &is, ReadPolicy &rp)
{
    std::string s;
    is >> s;
    if (s == "ReadShared")         rp = ReadShared;
    else if (s == "ReadUnordered") rp = ReadUnordered;
    else                           rp = UnspecifiedReadPolicy;
    return is;
}

} // namespace RTT
