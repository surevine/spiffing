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
        Tag(TagSet & tagSet, TagType tagType, InformativeEncoding t7enc, std::string const & name);
        std::string const & name() const {
          return m_name;
        }
        void addCategory(std::shared_ptr<Category> const &);
        std::shared_ptr<Tag> clone() const;
        TagType tagType() const {
          return m_tagType;
        }
        InformativeEncoding informativeEncoding() const {
          return m_t7enc;
        }

        bool hasMarkings() const {
          return m_markings != nullptr;
        }
        Markings const & markings() const {
          return *m_markings;
        }
        void markings(std::unique_ptr<Markings> && m) {
          m_markings = std::move(m);
        }
        TagSet const & tagSet() const {
          return m_tagSet;
        }

    private:
        std::string m_name;
        std::map<Lacv,std::shared_ptr<Category>> m_categories;
        TagType m_tagType;
        InformativeEncoding m_t7enc;
        TagSet & m_tagSet;
        std::unique_ptr<Markings> m_markings;
    };
}

#endif
