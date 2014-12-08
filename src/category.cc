#include <spiffing/category.h>

using namespace Spiffing;

Category::Category(Tag & tag, std::string const & name, Lacv const & lacv, size_t ordinal)
: m_tag(tag), m_name(name), m_lacv(lacv), m_ordinal(ordinal) {
}
