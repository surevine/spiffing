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

#ifndef SPIFFING_EQUIVCLASS_H
#define SPIFFING_EQUIVCLASS_H


#include <spiffing/constants.h>
#include <spiffing/categorygroup.h>

namespace Spiffing {
    class EquivClassification {
    public:
        EquivClassification(std::string const & policy_id, lacv_t lacv);
        void addRequiredCategory(std::unique_ptr<CategoryGroup> && reqCat);
        void compile();
        std::unique_ptr<Label> create() const;
        bool matches(Label const &) const;
        std::string const & policy_id() const {
            return m_policy_id;
        }
    private:
        std::string const m_policy_id;
        std::set<std::unique_ptr<CategoryGroup>> m_reqs;
        lacv_t m_class;
    };
}


#endif //SPIFFING_EQUIVCLASS_H
