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

#include <spiffing/category.h>
#include <spiffing/classification.h>
#include <spiffing/tag.h>
#include <spiffing/label.h>
#include <spiffing/equivcat.h>

using namespace Spiffing;

Category::Category(Tag & tag, std::string const & name, Lacv const & lacv, size_t ordinal)
: m_lacv(lacv), m_name(name), m_tag(tag), m_ordinal(ordinal) {
}

void Category::excluded(Classification const & c) {
  auto ins = m_excludedClass.insert(c.lacv());
  if (!ins.second) {
    throw std::runtime_error("Duplicate excluded classification in category");
  }
}

void Category::excluded(std::unique_ptr<CategoryData> && cd) {
  m_excluded.insert(std::move(cd));
}

void Category::required(std::unique_ptr<CategoryGroup> && cg) {
  m_required.insert(std::move(cg));
}

bool Category::valid(Label const & label) const {
  if (m_excludedClass.find(label.classification().lacv()) != m_excludedClass.end()) return false;
  for (auto & excl : m_excluded) {
    if (excl->matches(label)) return false;
  }
  for (auto & req : m_required) {
    if (!req->matches(label)) return false;
  }
  return true;
}

void Category::compile(Spif const & spif) {
  for (auto & excl : m_excluded) {
    excl->compile(spif);
  }
  for (auto & req : m_required) {
    req->compile(spif);
  }
}

void Category::encryptEquiv(std::shared_ptr<EquivCat> const & equivCat) {
  m_encryptEquivs.insert(std::make_pair(equivCat->policy_id(), equivCat));
}

void Category::encrypt(Label &label, std::string const &policy_id) const {
  auto i = m_encryptEquivs.lower_bound(policy_id);
  auto end = m_encryptEquivs.upper_bound(policy_id);
  if (i == end) throw std::runtime_error("No equivalence to " + m_name);
  for(; i != end; ++i) {
    (*i).second->apply(label);
  }
}