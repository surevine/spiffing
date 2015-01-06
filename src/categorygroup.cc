#include <spiffing/categorygroup.h>

#include <spiffing/categorydata.h>

using namespace Spiffing;

CategoryGroup::CategoryGroup(OperationType opType) : m_opType(opType) {}

bool CategoryGroup::matches(Category const & c) const {
  bool found{false};
  for (auto const & cd : m_categoryData) {
    if (cd->matches(c)) {
      switch (m_opType) {
      case OperationType::onlyOne:
        if (found) return false;
        break;
      case OperationType::oneOrMore:
        return true;
      }
      found = true;
    } else if (m_opType == OperationType::all) {
      return false;
    }
  }
  return found;
}

void CategoryGroup::addCategoryData(std::unique_ptr<CategoryData> cd) {
  m_categoryData.insert(std::move(cd));
}
