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
