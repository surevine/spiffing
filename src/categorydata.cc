#include <spiffing/categorydata.h>
#include <spiffing/category.h>
#include <spiffing/tag.h>
#include <spiffing/tagset.h>

using namespace Spiffing;

CategoryData::CategoryData(std::string const & tagSetRef, TagType tagType)
  : m_tagSetRef(tagSetRef), m_tagType(tagType), m_all(true) {
}

CategoryData::CategoryData(std::string const & tagSetRef, TagType tagType, Lacv const & l)
  : m_tagSetRef(tagSetRef), m_tagType(tagType), m_lacv(l), m_all(false) {
}

bool CategoryData::matches(Category const & c) const {
  if (c.tag().tagType() != m_tagType) {
    return false;
  }
  if (!m_all && c.lacv() != m_lacv) {
    return false;
  }
  if (c.tag().tagSet().name() != m_tagSetRef) {
    return false;
  }
  return true;
}
