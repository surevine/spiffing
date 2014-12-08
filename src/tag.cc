#include <spiffing/tag.h>
#include <spiffing/category.h>
#include <spiffing/tagset.h>

using namespace Spiffing;

Tag::Tag(TagSet & tagSet, TagType tagType, std::string const & name)
: m_tagSet(tagSet), m_tagType(tagType), m_name{name} {
}

void Tag::addCategory(std::shared_ptr<Category> const & c) {
  m_categories[c->lacv()] = c;
  m_tagSet.addCategory(*this, c);
}
