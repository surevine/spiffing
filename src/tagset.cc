#include <spiffing/tagset.h>
#include <spiffing/tag.h>
#include <spiffing/category.h>

using namespace Spiffing;

TagSet::TagSet(std::string const & id, std::string const & name)
: m_id{id}, m_name{name} {
}

void TagSet::addTag(std::shared_ptr<Tag> const & t) {
	m_tags[t->name()] = t;
}

void TagSet::addCategory(Tag const & tag, std::shared_ptr<Category> const & cat) {
	m_cats[std::make_pair(tag.tagType(), cat->lacv())] = cat;
}
