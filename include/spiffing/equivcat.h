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

#ifndef SPIFFING_EQUIVCAT_H
#define SPIFFING_EQUIVCAT_H

#include <spiffing/constants.h>
#include <spiffing/lacv.h>
#include <spiffing/categoryref.h>

namespace Spiffing {
    class EquivCat {
    public:
        EquivCat(std::string const & policy_id, std::string const & tagSetId, TagType tagType, Lacv lacv);
        EquivCat(std::string const & policy_id); // discard
        void apply(Label &) const;
        std::string const & policy_id() const {
            return m_policy_id;
        }

    private:
        bool m_discard;
        std::string const m_policy_id;
        std::string const m_tagSetId;
        TagType const m_tagType;
        Lacv const m_lacv;
        std::shared_ptr<Category> m_cat;
        void compile(Spif const &);
    };
}

#endif //SPIFFING_EQUIVCAT_H
