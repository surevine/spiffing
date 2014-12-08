#ifndef SPIFFING_TAG_H
#define SPIFFING_TAG_H

#include <spiffing/constants.h>
#include <spiffing/lacv.h>
#include <spiffing/marking.h>
#include <set>
#include <memory>
#include <map>

namespace Spiffing {
    class Tag {
    public:
        Tag(TagSet & tagSet, TagType tagType, std::string const & name);
        std::string const & name() const {
          return m_name;
        }
        void addCategory(std::shared_ptr<Category> const &);
        std::shared_ptr<Tag> clone() const;
        TagType tagType() const {
          return m_tagType;
        }

        bool hasMarking() const {
          return m_marking != nullptr;
        }
        Marking const & marking() const {
          return *m_marking;
        }
        Marking const & marking(std::unique_ptr<Marking> m) {
          m_marking.swap(m);
          return *m_marking;
        }
        TagSet const & tagSet() const {
          return m_tagSet;
        }

    private:
        std::string m_name;
        std::map<Lacv,std::shared_ptr<Category>> m_categories;
        TagType m_tagType;
        TagSet & m_tagSet;
        std::unique_ptr<Marking> m_marking;
    };
}

#endif
