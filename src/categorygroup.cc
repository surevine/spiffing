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

#include <spiffing/categorygroup.h>

#include <spiffing/categorydata.h>

using namespace Spiffing;

CategoryGroup::CategoryGroup(OperationType opType) : m_opType(opType) {}

bool CategoryGroup::matches(Label const & l) const {
  bool found{false};
  for (auto const & cd : m_categoryData) {
    if (cd->matches(l)) {
      switch (m_opType) {
      case OperationType::onlyOne:
        if (found) return false;
        break;
      case OperationType::oneOrMore:
        return true;
      case OperationType::all:
        ;
      }
      found = true;
    } else if (m_opType == OperationType::all) {
      return false;
    }
  }
  return found;
}

void CategoryGroup::addCategoryData(std::unique_ptr<CategoryData> &&cd) {
  m_categoryData.insert(std::move(cd));
}

void CategoryGroup::compile(Spif const & spif) {
  for (auto & cd : m_categoryData) {
    cd->compile(spif);
  }
}

void CategoryGroup::fixup(Label &l) const {
  for (auto & cd : m_categoryData) {
    cd->fixup(l);
  }
}