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

#ifndef SPIFFING_CATREF_H
#define SPIFFING_CATREF_H

#include <spiffing/constants.h>
#include <spiffing/category.h>
#include <memory>

namespace Spiffing {
  class CategoryRef {
  public:
    CategoryRef(std::shared_ptr<Category> const & cat) : m_cat(cat) {}
    CategoryRef(CategoryRef const & other) : m_cat(other.m_cat) {}
    CategoryRef(CategoryRef && other) : m_cat(std::move(other.m_cat)) {}
    Category & operator * () { return *m_cat; }
    Category const & operator * () const { return *m_cat; }
    Category * operator -> () { return &*m_cat; }
    Category const * operator -> () const { return &*m_cat; }
    bool operator <(CategoryRef const & other) const {
      return *m_cat < *other;
    }

  private:
    std::shared_ptr<Category> m_cat;
  };
}

#endif
