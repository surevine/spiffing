#ifndef SPIFFING_CATEGORY_H
#define SPIFFING_CATEGORY_H

#include <spiffing/constants.h>
#include <spiffing/lacv.h>
#include <string>

namespace Spiffing {
    class Category {
    public:
        Category(Tag & tag, std::string const & name, Lacv const & lacv, size_t ordinal);

        std::string const & name() const {
          return m_name;
        }
        Lacv const & lacv() const {
          return m_lacv;
        }
        bool operator <(Category const & other) const {
          return m_ordinal < other.m_ordinal;
        }
        Tag const & tag() const {
          return m_tag;
        }

    private:
        Lacv m_lacv;
        std::string m_name;
        Tag & m_tag;
        size_t m_ordinal;
    };
}

#endif
