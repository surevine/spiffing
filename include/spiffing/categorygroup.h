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

#ifndef SPIFFING_CATEGORYGROUP_H
#define SPIFFING_CATEGORYGROUP_H

#include <spiffing/constants.h>
#include <spiffing/categorydata.h>

#include <set>
#include <memory>

namespace Spiffing {
  class CategoryData;
  class CategoryGroup {
  public:
    CategoryGroup(OperationType opType);

    bool matches(Label const &) const;
    void compile(Spif const & spif);

    void addCategoryData(std::unique_ptr<CategoryData> &&);
    void fixup(Label & l) const;
  private:
    std::set<std::unique_ptr<CategoryData>> m_categoryData;
    OperationType m_opType;
  };
}

#endif
