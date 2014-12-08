#ifndef SPIFFING_TAGSET_H
#define SPIFFING_TAGSET_H

#include <string>
#include <spiffing/constants.h>
#include <spiffing/lacv.h>
#include <memory>
#include <map>

namespace Spiffing {
    class TagSet {
    public:
        TagSet(std::string const & id, std::string const & name);

        std::string const & id() const {
            return m_id;
        }
        std::string const & name() const {
            return m_name;
        }
        void addTag(std::shared_ptr<Tag> const &);

        std::shared_ptr<Category> const & categoryLookup(TagType type, Lacv lacv) const {
          auto i = m_cats.find(std::make_pair(type, lacv));
          if (i == m_cats.end()) throw std::runtime_error("Cannot find the tagType/LACV");
          return (*i).second;
        }

        void addCategory(Tag const &, std::shared_ptr<Category> const &);
    private:
        std::string m_id;
        std::string m_name;
        std::map<std::string,std::shared_ptr<Tag>> m_tags;
        std::map<std::pair<TagType, Lacv>, std::shared_ptr<Category>> m_cats;
    };
}

#endif
