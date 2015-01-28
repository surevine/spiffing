/***

Copyright 2014 Dave Cridland
Copyright 2014 Surevine Ltd

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
        void addMarking(std::unique_ptr<Marking> marking);
    private:
        std::string m_id;
        std::string m_name;
        std::map<std::string,std::shared_ptr<Tag>> m_tags;
        std::map<std::pair<TagType, Lacv>, std::shared_ptr<Category>> m_cats;
        std::unique_ptr<Marking> m_marking;
    };
}

#endif
