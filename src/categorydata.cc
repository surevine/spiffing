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

#include <spiffing/categorydata.h>
#include <spiffing/category.h>
#include <spiffing/tag.h>
#include <spiffing/tagset.h>
#include <spiffing/categoryref.h>
#include <spiffing/spif.h>
#include <spiffing/label.h>

using namespace Spiffing;

CategoryData::CategoryData(std::string const & tagSetRef, TagType tagType)
  : m_tagSetRef(tagSetRef), m_tagType(tagType), m_all(true) {
}

CategoryData::CategoryData(std::string const & tagSetRef, TagType tagType, Lacv const & l)
  : m_tagSetRef(tagSetRef), m_tagType(tagType), m_lacv(l), m_all(false) {
}

bool CategoryData::matches(Label const & l) const {
  bool ok = false;
  for (auto c : m_cats) if (l.hasCategory(c)) {
    ok = true;
    break;
  }
  return ok;
}

void CategoryData::compile(Spif const & spif) {
  auto tagSet = spif.tagSetLookupByName(m_tagSetRef);
  if (m_all) {
    m_cats = tagSet->categories(m_tagType);
  } else {
    m_cats.insert(tagSet->categoryLookup(m_tagType, m_lacv));
  }
}

void CategoryData::fixup(Label & l) const {
  if (m_cats.empty()) {
    const_cast<CategoryData *>(this)->compile(l.policy());
  }
  for (auto & cat : m_cats) {
    l.addCategory(cat.ptr());
  }
}