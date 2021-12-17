/***

Copyright 2014-2015 Dave Cridland
Copyright 2014-2015 Surevine Ltd

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

#ifndef SPIFFING_TAGSET_H
#define SPIFFING_TAGSET_H

#include <string>
#include <spiffing/constants.h>
#include <spiffing/lacv.h>
#include <memory>
#include <map>
#include <set>
#include <stdexcept>

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
        std::shared_ptr<Category> const & categoryLookup(TagType type, std::string const & name) const {
            auto i = m_catnames.find(std::make_pair(type, name));
            if (i == m_catnames.end()) throw std::runtime_error("Cannot find the tagType/name: " + name);
            return (*i).second;
        }

        void addCategory(Tag const &, std::shared_ptr<Category> const &);
        std::set<CategoryRef> categories(TagType type) const;
        void compile(Spif const &);
        bool hasMarkings() const {
            return m_markings != nullptr;
        }
        Markings const & markings() const {
            return *m_markings;
        }
        void markings(std::unique_ptr<Markings> && m) {
            m_markings = std::move(m);
        }
    private:
        std::string m_id;
        std::string m_name;
        std::map<std::string,std::shared_ptr<Tag>> m_tags;
        std::map<std::pair<TagType, Lacv>, std::shared_ptr<Category>> m_cats;
        std::map<std::pair<TagType, std::string>, std::shared_ptr<Category>> m_catnames;
        std::unique_ptr<Markings> m_markings;
    };
}

#endif
