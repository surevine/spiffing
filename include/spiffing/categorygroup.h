#ifndef SPIFFING_CATEGORYGROUP_H
#define SPIFFING_CATEGORYGROUP_H

#include <spiffing/constants.h>

#include <set>
#include <memory>

namespace Spiffing {
  class CategoryData;
  class CategoryGroup {
  public:
    CategoryGroup(OperationType opType);

    bool matches(Category const &) const;

    void addCategoryData(std::unique_ptr<CategoryData>);
  private:
    std::set<std::unique_ptr<CategoryData>> m_categoryData;
    OperationType m_opType;
  };
}

#endif
