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

#include <spiffing/tag.h>
#include <spiffing/category.h>
#include <spiffing/tagset.h>
#include <spiffing/classification.h>

using namespace Spiffing;

Tag::Tag(TagSet & tagSet, TagType tagType, std::string const & name)
: m_tagSet(tagSet), m_tagType(tagType), m_name{name} {
}

void Tag::addCategory(std::shared_ptr<Category> const & c) {
  m_categories[c->lacv()] = c;
  m_tagSet.addCategory(*this, c);
}

bool Tag::valid(Classification const & c) const {
  return m_excludedClass.find(c.lacv()) == m_excludedClass.end();
}

void Tag::excluded(Classification const & c) {
  auto ins = m_excludedClass.insert(c.lacv());
  if (!ins.second) {
    // Duplicate, but identical, excludedClass?
  }
}
