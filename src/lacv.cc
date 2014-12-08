/***

Copyright 2014 Dave Cridland
Copyright 2014 Surevine Ltd

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

#include <spiffing/lacv.h>
#include <sstream>
#include <string>
#include <INTEGER.h>
#include <spiffing/catutils.h>

using namespace Spiffing;

Lacv::Lacv(std::string const & name) : m_sz_valid(false), m_name(name) {}

Lacv::Lacv(size_t iname) : m_sz(iname), m_sz_valid(true), m_name() {
  Asn<INTEGER_t> i(&asn_DEF_INTEGER);
  std::stringstream tmp;
  tmp << iname;
  std::string s = tmp.str();
  std::string tmps = "<INTEGER>" + s + "</INTEGER>";
  xer_decode(0, &asn_DEF_INTEGER, i.addr(), tmps.data(), tmps.size());
  m_name = std::string{reinterpret_cast<char *>(i->buf), static_cast<size_t>(i->size)};
  m_str = s;
}

Lacv Lacv::parse(std::string const & lstr) {
  Asn<INTEGER_t> i(&asn_DEF_INTEGER);
  std::string tmp = "<INTEGER>" + lstr + "</INTEGER>";
  xer_decode(0, &asn_DEF_INTEGER, i.addr(), tmp.data(), tmp.size());
  Lacv lacv{std::string{reinterpret_cast<char *>(i->buf), static_cast<size_t>(i->size)}};
  lacv.m_str = lstr;
  return lacv;
}

void Lacv::genString() const {
  Asn<INTEGER_t> i(&asn_DEF_INTEGER);
  i.alloc();
  i->buf = reinterpret_cast<uint8_t *>(const_cast<char *>(m_name.data()));
  i->size = m_name.size();
  unsigned long tmpsz;
  if (!m_sz_valid && !asn_INTEGER2ulong(&*i, &tmpsz)) {
    m_sz_valid = true;
    m_sz = tmpsz;
  }
  asn_DEF_INTEGER.xer_encoder(&asn_DEF_INTEGER, &*i, 0, XER_F_BASIC, Internal::write_to_string, &m_str);
  i->buf = nullptr;
}
