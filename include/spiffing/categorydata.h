/***

Copyright 2015 Dave Cridland
Copyright 2015 Surevine Ltd

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

#ifndef SPIFFING_CATEGORYDATA_H
#define SPIFFING_CATEGORYDATA_H

#include <spiffing/constants.h>
#include <spiffing/lacv.h>
#include <spiffing/categoryref.h>

#include <string>
#include <set>

namespace Spiffing {
  class CategoryData {
  public:
    CategoryData(std::string const & tagSetRef, TagType tagType, Lacv const & l);
    CategoryData(std::string const & tagSetRef, TagType tagType);

    void compile(Spif const &);
    bool matches(Label const &) const;
    void fixup(Label &) const;
  private:
    std::set<CategoryRef> m_cats;
    std::string m_tagSetRef;
    TagType m_tagType;
    Lacv m_lacv;
    bool m_all;
  };
}

#endif
