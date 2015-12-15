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

#include <spiffing/equivcat.h>
#include <spiffing/label.h>
#include <spiffing/spif.h>
#include <spiffing/tagset.h>

using namespace Spiffing;

EquivCat::EquivCat(std::string const & policy_id, std::string const & tagSetId, TagType tagType, Lacv lacv)
    : m_discard(false), m_policy_id(policy_id), m_tagSetId(tagSetId), m_tagType(tagType), m_lacv(lacv) {
}

EquivCat::EquivCat(std::string const & policy_id)
    : m_discard(true), m_policy_id(policy_id), m_tagType(TagType::informative) {
}

void EquivCat::apply(Label & label) const {
    if (m_discard) return;
    if (!m_cat) const_cast<EquivCat*>(this)->compile(label.policy());
    label.addCategory(m_cat);
}

void EquivCat::compile(Spif const & policy) {
    m_cat = policy.tagSetLookup(m_tagSetId)->categoryLookup(m_tagType, m_lacv);
}