#ifndef SPIFFING_CATEGORYDATA_H
#define SPIFFING_CATEGORYDATA_H

#include <spiffing/constants.h>
#include <spiffing/lacv.h>

#include <string>

namespace Spiffing {
  class CategoryData {
  public:
    CategoryData(std::string const & tagSetRef, TagType tagType, Lacv const & l);
    CategoryData(std::string const & tagSetRef, TagType tagType);

    bool matches(Category const &) const;
  private:
    std::string m_tagSetRef;
    TagType m_tagType;
    Lacv m_lacv;
    bool m_all;
  };
}

#endif
