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

#include <spiffing/spiffing.h>
#include <spiffing/equivclass.h>
#include <spiffing/label.h>

using namespace Spiffing;

EquivClassification::EquivClassification(std::string const &policy_id, lacv_t lacv)
    : m_policy_id(policy_id), m_class(lacv) {
}

void EquivClassification::addRequiredCategory(std::unique_ptr<CategoryGroup> &&reqCat) {
    m_reqs.insert(std::move(reqCat));
}

std::unique_ptr<Label> EquivClassification::create() const {
    std::shared_ptr<Spif> spif = Site::site().spif(m_policy_id);
    std::unique_ptr<Label> label = std::make_unique<Label>(spif, m_class);
    for (auto & i : m_reqs) {
        i->fixup(*label);
    }
    return label;
}