/***

Copyright 2014-2015 Dave Cridland
Copyright 2014-2015 Surevine Ltd

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

***/

#ifndef SPIFFING_LACV_H
#define SPIFFING_LACV_H

#include <spiffing/constants.h>
#include <string>

namespace Spiffing {
    class Lacv {
    public:
        Lacv(std::string const &);
        Lacv(std::size_t);
        Lacv();

        static Lacv parse(std::string const &);

        std::string const & text() const {
          return m_name;
        }
        bool ascii() const;
        bool operator <(Lacv const & l) const {
          return m_name < l.m_name;
        }
        bool operator ==(Lacv const & l) const {
          return m_name == l.m_name;
        }
        bool operator !=(Lacv const & l) const {
          return m_name != l.m_name;
        }

        void genString() const;
        std::string const & toString() const {
          if (m_str.empty()) genString();
          return m_str;
        }

        std::size_t operator % (std::size_t m) const {
          if (!m_sz_valid) genString();
          return m_sz % m;
        }

        std::size_t operator / (std::size_t d) const {
          if (!m_sz_valid) genString();
          return m_sz / d;
        }
    private:
        void parse(std::size_t sz);
        mutable std::size_t m_sz = 0;
        mutable bool m_sz_valid = false;
        std::string m_name;
        mutable std::string m_str;
    };
}

#endif
