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

#ifndef SPIFFING_CATEGORY_H
#define SPIFFING_CATEGORY_H

#include <spiffing/constants.h>
#include <spiffing/lacv.h>
#include <string>
#include <set>
#include <memory>

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
        bool valid(Label const & label) const;
        void compile(Spif const & spif);
        void excluded(Classification const & c);
        void excluded(std::unique_ptr<CategoryData> && cd);
        void required(std::unique_ptr<CategoryGroup> && cg);

    private:
        Lacv m_lacv;
        std::string m_name;
        Tag & m_tag;
        size_t m_ordinal;
        std::set<lacv_t> m_excludedClass;
        std::set<std::unique_ptr<CategoryGroup>> m_required;
        std::set<std::unique_ptr<CategoryData>> m_excluded;
    };
}

#endif
