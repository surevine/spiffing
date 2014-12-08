#ifndef SPIFFING_CATREF_H
#define SPIFFING_CATREF_H

#include <spiffing/constants.h>
#include <spiffing/category.h>
#include <memory>

namespace Spiffing {
  class CategoryRef {
  public:
    CategoryRef(std::shared_ptr<Category> const & cat) : m_cat(cat) {}
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
