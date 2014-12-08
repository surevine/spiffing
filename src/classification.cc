#include <spiffing/classification.h>

using namespace Spiffing;

Classification::Classification(lacv_t lacv, std::string const & name, unsigned long hierarchy, bool obsolete)
: m_lacv{lacv}, m_name{name}, m_hierarchy{hierarchy}, m_obsolete{obsolete} {
}

bool Classification::operator < (Classification const & c) const {
	return m_hierarchy < c.m_hierarchy;
}
