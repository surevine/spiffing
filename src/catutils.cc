/***

Copyright 2014 Dave Cridland
Copyright 2014 Surevine Ltd

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

#include <spiffing/catutils.h>
#include <spiffing/tag.h>
#include <spiffing/tagset.h>
#include <spiffing/label.h>
#include <spiffing/clearance.h>
#include <spiffing/spif.h>
// NATO:
#include <SecurityCategory.h>
#include <PermissiveTag.h>
#include <RestrictiveTag.h>
// MISSI:
#include <MissiSecurityCategories.h>

#include <INTEGER.h>
#include <TagType7Data.h>
#include <StandardSecurityLabel.h>
#include <NamedTagSet.h>
#include <SecurityCategories.h>
#include <SecurityTag.h>

// SSLP
#include <SSLPrivileges.h>

using namespace Spiffing;

int Internal::write_to_string(const void * buffer, size_t size, void * app_key) {
  std::string * s = reinterpret_cast<std::string *>(app_key);
  s->append(reinterpret_cast<const char *>(buffer), size);
  return 1; // Something non-negative.
}

std::string Internal::oid2str(OBJECT_IDENTIFIER_t * oid) {
  std::string tmp;
  asn_DEF_OBJECT_IDENTIFIER.xer_encoder(&asn_DEF_OBJECT_IDENTIFIER, oid, 0, XER_F_BASIC, write_to_string, &tmp);
  return tmp;
}

OBJECT_IDENTIFIER_t * Internal::str2oid(std::string const & oid) {
  Asn<OBJECT_IDENTIFIER_t> o(&asn_DEF_OBJECT_IDENTIFIER);
  std::string tmp = "<OBJECT_IDENTIFIER>" + oid + "</OBJECT_IDENTIFIER>";
  xer_decode(0, &asn_DEF_OBJECT_IDENTIFIER, o.addr(), tmp.data(), tmp.size());
  return o.release();
}

void Internal::str2oid(std::string const & oid, OBJECT_IDENTIFIER_t * o) {
  std::string tmp = "<OBJECT_IDENTIFIER>" + oid + "</OBJECT_IDENTIFIER>";
  xer_decode(0, &asn_DEF_OBJECT_IDENTIFIER, (void **)&o, tmp.data(), tmp.size());
}

namespace {
  void setbit(BIT_STRING_t & bitString, Lacv const & bit) {
    std::size_t sz = bitString.size;
    if (sz == 0) {
      sz = 4;
      bitString.buf = (unsigned char *) calloc(sizeof(unsigned char), sz);
      bitString.size = sz;
    }
    if (sz <= bit / 8) {
      std::size_t old_sz = sz;
      while (sz <= bit / 8) sz += 4;
      bitString.buf = (unsigned char *)realloc(bitString.buf, sz);
      memset(bitString.buf + old_sz, 0, sz - old_sz);
      bitString.size = sz;
    }
    bitString.buf[bit / 8] |= (1 << (7 - (bit % 8)));
    bitString.bits_unused = (8 * bitString.size) - (8 * (bitString.size - (bit / 8)) + (1 << (bit % 8)));
  }

  typedef A_SET_OF(SecurityAttribute_t) SecurityAttributes;
  void setatt(SecurityAttributes & atts, Lacv const & att) {
    std::size_t sz = atts.size;
    if (sz == 0) {
      atts.size = sz = 4;
      atts.array = (INTEGER_t **)calloc(sz * sizeof(INTEGER_t *), sz);
    }
    if (sz <= atts.count) {
      std::size_t old_sz = sz;
      while (sz <= atts.count) sz += 4;
      atts.array = (INTEGER_t **)realloc(atts.array, sz * sizeof(INTEGER_t *));
      memset(atts.array + old_sz, 0, (sz - old_sz) * sizeof(INTEGER_t *));
      atts.size = sz;
    }
    Asn<INTEGER_t> lacv(&asn_DEF_INTEGER);
    lacv.alloc();
    lacv->buf = (unsigned char *)strdup(att.text().c_str());
    lacv->size = att.text().size();
    atts.array[atts.count] = lacv.release();
    ++atts.count;
  }

  template<typename T> void catencodebits(Asn<SecurityCategories_t> & asn_cats, std::set<CategoryRef> const & cats, std::string const & oid, asn_TYPE_descriptor_t * asn_type, std::set<CategoryRef>::const_iterator & i) {
    auto const & cat = *i;
    std::string tagSetId = cat->tag().tagSet().id();
    TagType ttype = cat->tag().tagType();
    Asn<SecurityCategory> asn_cat(&asn_DEF_SecurityCategory);
    asn_cat.alloc();
    Internal::str2oid(oid, &asn_cat->type);
    Asn<T> asn_tag(asn_type);
    asn_tag.alloc();
    Internal::str2oid(tagSetId, &asn_tag->tagName);
    while (i != cats.end() && tagSetId == (*i)->tag().tagSet().id() && ttype == (*i)->tag().tagType()) {
      setbit(asn_tag->attributeFlags, (*i)->lacv());
      ++i;
    }
    ANY_fromType(&asn_cat->value, asn_type, &*asn_tag);
    asn_cats->list.array[asn_cats->list.count] = asn_cat.release();
    asn_cats->list.count++;
  }

  void catencodeenum(Asn<SecurityCategories_t> & asn_cats, std::set<CategoryRef> const & cats, std::string const & oid, std::set<CategoryRef>::const_iterator & i) {
    auto const & cat = *i;
    std::string tagSetId = cat->tag().tagSet().id();
    TagType ttype = cat->tag().tagType();
    Asn<SecurityCategory> asn_cat(&asn_DEF_SecurityCategory);
    asn_cat.alloc();
    Internal::str2oid(oid, &asn_cat->type);
    Asn<EnumeratedTag_t> asn_tag(&asn_DEF_EnumeratedTag);
    asn_tag.alloc();
    Internal::str2oid(tagSetId, &asn_tag->tagName);
    while (i != cats.end() && tagSetId == (*i)->tag().tagSet().id() && ttype == (*i)->tag().tagType()) {
      setatt(reinterpret_cast<SecurityAttributes &>(asn_tag->attributeList.list), (*i)->lacv());
      ++i;
    }
    ANY_fromType(&asn_cat->value, &asn_DEF_EnumeratedTag, &*asn_tag);
    asn_cats->list.array[asn_cats->list.count] = asn_cat.release();
    asn_cats->list.count++;
  }

    void catencodeinform(Asn<SecurityCategories_t> & asn_cats, std::set<CategoryRef> const & cats, std::set<CategoryRef>::const_iterator & i) {
      auto const & cat = *i;
      std::string tagSetId = cat->tag().tagSet().id();
      TagType ttype = cat->tag().tagType();
      Asn<SecurityCategory> asn_cat(&asn_DEF_SecurityCategory);
      asn_cat.alloc();
      Internal::str2oid(OID::NATO_Informative, &asn_cat->type);
      Asn<InformativeTag_t> asn_tag(&asn_DEF_InformativeTag);
      asn_tag.alloc();
      Internal::str2oid(tagSetId, &asn_tag->tagName);
      if (cat->tag().informativeEncoding() == InformativeEncoding::enumerated) {
        while (i != cats.end() && tagSetId == (*i)->tag().tagSet().id() && ttype == (*i)->tag().tagType()) {
          setatt(reinterpret_cast<SecurityAttributes &>(asn_tag->field.choice.securityAttributes.list), (*i)->lacv());
          ++i;
        }
        asn_tag->field.present = FreeFormField_PR_securityAttributes;
      } else {
        while (i != cats.end() && tagSetId == (*i)->tag().tagSet().id() && ttype == (*i)->tag().tagType()) {
          setbit(asn_tag->field.choice.bitSetAttributes, (*i)->lacv());
          ++i;
        }
        asn_tag->field.present = FreeFormField_PR_bitSetAttributes;
      }
      ANY_fromType(&asn_cat->value, &asn_DEF_InformativeTag, &*asn_tag);
      asn_cats->list.array[asn_cats->list.count] = asn_cat.release();
      asn_cats->list.count++;
    }
    void missi_catencodeinform(Asn<NamedTagSet> & asn_cats, std::set<CategoryRef> const & cats, std::set<CategoryRef>::const_iterator & i) {
      auto const & cat = *i;
      TagType ttype = (*i)->tag().tagType();
      std::string tagSetId = cat->tag().tagSet().id();
      Asn<SecurityTag> asn_tag(&asn_DEF_SecurityTag);
      asn_tag.alloc();
      Asn<TagType7Data> asn_t7(&asn_DEF_TagType7Data);
      asn_t7.alloc();
      if (cat->tag().informativeEncoding() == InformativeEncoding::enumerated) {
        while (i != cats.end() && tagSetId == (*i)->tag().tagSet().id() && ttype == (*i)->tag().tagType()) {
          setatt(reinterpret_cast<SecurityAttributes &>(asn_t7->choice.securityAttributes.list), (*i)->lacv());
          ++i;
        }
        asn_t7->present = TagType7Data_PR_securityAttributes;
      } else {
        while (i != cats.end() && tagSetId == (*i)->tag().tagSet().id() && ttype == (*i)->tag().tagType()) {
          setbit(asn_t7->choice.bitSetAttributes, (*i)->lacv());
          ++i;
        }
        asn_t7->present = TagType7Data_PR_bitSetAttributes;
      }
      ANY_fromType((ANY_t*)&asn_tag->choice.freeFormField, &asn_DEF_TagType7Data, &*asn_t7);
      asn_tag->present = SecurityTag_PR_freeFormField;
      asn_cats->securityTags.list.array[asn_cats->securityTags.list.count] = asn_tag.release();
      asn_cats->securityTags.list.count++;
    }
    void missi_catencodeenum(Asn<NamedTagSet> & asn_cats, std::set<CategoryRef> const & cats, std::set<CategoryRef>::const_iterator & i) {
      auto const & cat = *i;
      TagType ttype = (*i)->tag().tagType();
      std::string tagSetId = cat->tag().tagSet().id();
      Asn<SecurityTag> asn_tag(&asn_DEF_SecurityTag);
      asn_tag.alloc();
      while (i != cats.end() && tagSetId == (*i)->tag().tagSet().id() && ttype == (*i)->tag().tagType()) {
        setatt(reinterpret_cast<SecurityAttributes &>(asn_tag->choice.enumeratedAttributes.attributeFlags.list),
               (*i)->lacv());
        ++i;
      }
      asn_tag->present = SecurityTag_PR_enumeratedAttributes;
      asn_cats->securityTags.list.array[asn_cats->securityTags.list.count] = asn_tag.release();
      asn_cats->securityTags.list.count++;
    }
    void missi_catencodepbits(Asn<NamedTagSet> & asn_cats, std::set<CategoryRef> const & cats, std::set<CategoryRef>::const_iterator & i) {
      auto const & cat = *i;
      TagType ttype = (*i)->tag().tagType();
      std::string tagSetId = cat->tag().tagSet().id();
      Asn<SecurityTag> asn_tag(&asn_DEF_SecurityTag);
      asn_tag.alloc();
      while (i != cats.end() && tagSetId == (*i)->tag().tagSet().id() && ttype == (*i)->tag().tagType()) {
        setbit(asn_tag->choice.permissivebitMap.attributeFlags,
               (*i)->lacv());
        ++i;
      }
      asn_tag->present = SecurityTag_PR_permissivebitMap;
      asn_cats->securityTags.list.array[asn_cats->securityTags.list.count] = asn_tag.release();
      asn_cats->securityTags.list.count++;
    }
    void missi_catencoderbits(Asn<NamedTagSet> & asn_cats, std::set<CategoryRef> const & cats, std::set<CategoryRef>::const_iterator & i) {
      auto const & cat = *i;
      TagType ttype = (*i)->tag().tagType();
      std::string tagSetId = cat->tag().tagSet().id();
      Asn<SecurityTag> asn_tag(&asn_DEF_SecurityTag);
      asn_tag.alloc();
      while (i != cats.end() && tagSetId == (*i)->tag().tagSet().id() && ttype == (*i)->tag().tagType()) {
        setbit(asn_tag->choice.restrictivebitMap.attributeFlags,
               (*i)->lacv());
        ++i;
      }
      asn_tag->present = SecurityTag_PR_restrictivebitMap;
      asn_cats->securityTags.list.array[asn_cats->securityTags.list.count] = asn_tag.release();
      asn_cats->securityTags.list.count++;
    }
    void sslp_catencodeenum(Asn<NamedTagSetPrivilege> & asn_cats, std::set<CategoryRef> const & cats, std::set<CategoryRef>::const_iterator & i) {
      auto const & cat = *i;
      TagType ttype = (*i)->tag().tagType();
      std::string tagSetId = cat->tag().tagSet().id();
      Asn<SecurityTagPrivilege> asn_tag(&asn_DEF_SecurityTagPrivilege);
      asn_tag.alloc();
      while (i != cats.end() && tagSetId == (*i)->tag().tagSet().id() && ttype == (*i)->tag().tagType()) {
        setatt(reinterpret_cast<SecurityAttributes &>(asn_tag->choice.enumeratedAttributes.list),
               (*i)->lacv());
        ++i;
      }
      asn_tag->present = SecurityTagPrivilege_PR_enumeratedAttributes;
      asn_cats->securityTagPrivileges.list.array[asn_cats->securityTagPrivileges.list.count] = asn_tag.release();
      asn_cats->securityTagPrivileges.list.count++;
    }
    void sslp_catencodepbits(Asn<NamedTagSetPrivilege> & asn_cats, std::set<CategoryRef> const & cats, std::set<CategoryRef>::const_iterator & i) {
      auto const & cat = *i;
      TagType ttype = (*i)->tag().tagType();
      std::string tagSetId = cat->tag().tagSet().id();
      Asn<SecurityTagPrivilege> asn_tag(&asn_DEF_SecurityTagPrivilege);
      asn_tag.alloc();
      while (i != cats.end() && tagSetId == (*i)->tag().tagSet().id() && ttype == (*i)->tag().tagType()) {
        setbit(asn_tag->choice.permissivebitMap,
               (*i)->lacv());
        ++i;
      }
      asn_tag->present = SecurityTagPrivilege_PR_permissivebitMap;
      asn_cats->securityTagPrivileges.list.array[asn_cats->securityTagPrivileges.list.count] = asn_tag.release();
      asn_cats->securityTagPrivileges.list.count++;
    }
    void sslp_catencoderbits(Asn<NamedTagSetPrivilege> & asn_cats, std::set<CategoryRef> const & cats, std::set<CategoryRef>::const_iterator & i) {
      auto const & cat = *i;
      TagType ttype = (*i)->tag().tagType();
      std::string tagSetId = cat->tag().tagSet().id();
      Asn<SecurityTagPrivilege> asn_tag(&asn_DEF_SecurityTagPrivilege);
      asn_tag.alloc();
      while (i != cats.end() && tagSetId == (*i)->tag().tagSet().id() && ttype == (*i)->tag().tagType()) {
        setbit(asn_tag->choice.restrictivebitMap,
               (*i)->lacv());
        ++i;
      }
      asn_tag->present = SecurityTagPrivilege_PR_restrictivebitMap;
      asn_cats->securityTagPrivileges.list.array[asn_cats->securityTagPrivileges.list.count] = asn_tag.release();
      asn_cats->securityTagPrivileges.list.count++;
    }
}

SecurityCategories * Internal::nato_catencode(std::set<CategoryRef> const &cats) {
  // Category Encoding.
  Asn<SecurityCategories_t> asn_cats(&asn_DEF_SecurityCategories);
  asn_cats.alloc();
  // Allocating too much memory here, could be more efficient.
  asn_cats->list.array = (SecurityCategory **)calloc(cats.size(), sizeof(SecurityCategory *));
  auto i = cats.begin();
  while (i != cats.end()) {
    auto const & cat = *i;
    switch (cat->tag().tagType()) {
    case TagType::permissive:
      catencodebits<PermissiveTag_t>(asn_cats, cats, OID::NATO_PermissiveBitmap, &asn_DEF_PermissiveTag, i);
      break;
    case TagType::restrictive:
      catencodebits<RestrictiveTag_t>(asn_cats, cats, OID::NATO_RestrictiveBitmap, &asn_DEF_RestrictiveTag, i);
      break;
    case TagType::enumeratedPermissive:
      catencodeenum(asn_cats, cats, OID::NATO_EnumeratedPermissive, i);
      break;
    case TagType::enumeratedRestrictive:
      catencodeenum(asn_cats, cats, OID::NATO_EnumeratedRestrictive, i);
      break;
    case TagType::informative:
      catencodeinform(asn_cats, cats, i);
      break;
    }
  }
  return asn_cats.release();
}

SecurityCategories * Internal::missi_catencode(const std::set<::Spiffing::CategoryRef> & cats) {
  // Category Encoding.
  Asn<SecurityCategories_t> asn_cats(&asn_DEF_SecurityCategories);
  asn_cats.alloc();
  Asn<SecurityCategory_t> asn_cat(&asn_DEF_SecurityCategories);
  asn_cat.alloc();
  Internal::str2oid(OID::MISSI, &asn_cat->type);
  Asn<MissiSecurityCategories_t> catinfo(&asn_DEF_MissiSecurityCategories);
  catinfo.alloc();
  catinfo->choice.prbacSecurityCategories.list.array = (NamedTagSet **)calloc(cats.size(), sizeof(NamedTagSet *));
  catinfo->choice.prbacSecurityCategories.list.size = cats.size();
  catinfo->present = MissiSecurityCategories_PR_prbacSecurityCategories;
  auto i = cats.begin();
  while (i != cats.end()) {
    std::string tagName = (*i)->tag().tagSet().id();
    Asn<NamedTagSet> asn_tagset(&asn_DEF_NamedTagSet);
    asn_tagset.alloc();
    asn_tagset->securityTags.list.array = (SecurityTag **)calloc(cats.size(), sizeof(SecurityTag *));
    asn_tagset->securityTags.list.size = cats.size();
    Internal::str2oid(tagName, &asn_tagset->tagSetName);
    while (tagName == (*i)->tag().tagSet().id()) {
      switch ((*i)->tag().tagType()) {
        case TagType::permissive:
          missi_catencodepbits(asn_tagset, cats, i);
          break;
        case TagType::restrictive:
          missi_catencoderbits(asn_tagset, cats, i);
          break;
        case TagType::enumeratedPermissive:
        case TagType::enumeratedRestrictive:
          missi_catencodeenum(asn_tagset, cats, i);
          break;
        case TagType::informative:
          missi_catencodeinform(asn_tagset, cats, i);
          break;
      }
      if (i == cats.end()) break;
    }
    catinfo->choice.prbacSecurityCategories.list.array[catinfo->choice.prbacSecurityCategories.list.count++] = asn_tagset.release();
  }
  asn_cats->list.array = (SecurityCategory **)calloc(1, sizeof(SecurityCategory *));
  ANY_fromType(&asn_cat->value, &asn_DEF_MissiSecurityCategories, &*catinfo);
  asn_cats->list.array[0] = asn_cat.release();
  asn_cats->list.size = 1;
  asn_cats->list.count = 1;
  return asn_cats.release();
}


SecurityCategories * Internal::sslp_catencode(const std::set<::Spiffing::CategoryRef> & cats) {
  // Category Encoding.
  Asn<SecurityCategories_t> asn_cats(&asn_DEF_SecurityCategories);
  asn_cats.alloc();
  Asn<SecurityCategory_t> asn_cat(&asn_DEF_SecurityCategories);
  asn_cat.alloc();
  Internal::str2oid(OID::SSLPrivilege, &asn_cat->type);
  Asn<SSLPrivileges> catinfo(&asn_DEF_SSLPrivileges);
  catinfo.alloc();
  catinfo->list.array = (NamedTagSetPrivilege **)calloc(cats.size(), sizeof(NamedTagSetPrivilege *));
  catinfo->list.size = cats.size();
  auto i = cats.begin();
  while (i != cats.end()) {
    std::string tagName = (*i)->tag().tagSet().id();
    Asn<NamedTagSetPrivilege> asn_tagset(&asn_DEF_NamedTagSetPrivilege);
    asn_tagset.alloc();
    asn_tagset->securityTagPrivileges.list.array = (SecurityTagPrivilege **)calloc(cats.size(), sizeof(SecurityTagPrivilege *));
    asn_tagset->securityTagPrivileges.list.size = cats.size();
    Internal::str2oid(tagName, &asn_tagset->tagSetName);
    while (tagName == (*i)->tag().tagSet().id()) {
      switch ((*i)->tag().tagType()) {
        case TagType::permissive:
          sslp_catencodepbits(asn_tagset, cats, i);
              break;
        case TagType::restrictive:
          sslp_catencoderbits(asn_tagset, cats, i);
              break;
        case TagType::enumeratedPermissive:
        case TagType::enumeratedRestrictive:
          sslp_catencodeenum(asn_tagset, cats, i);
              break;
        default:
          ++i;
      }
      if (i == cats.end()) break;
    }
    catinfo->list.array[catinfo->list.count++] = asn_tagset.release();
  }
  asn_cats->list.array = (SecurityCategory **)calloc(1, sizeof(SecurityCategory *));
  ANY_fromType(&asn_cat->value, &asn_DEF_SSLPrivileges, &*catinfo);
  asn_cats->list.array[0] = asn_cat.release();
  asn_cats->list.size = 1;
  asn_cats->list.count = 1;
  return asn_cats.release();
}

void Internal::parse_missi_cat(Label & label, ANY * any) {
  if (any->size == 0) {
    throw std::runtime_error("MISSI tag missing or corrupt");
  }
  Asn<MissiSecurityCategories> missi(&asn_DEF_MissiSecurityCategories);
  auto r = ANY_to_type(any, &asn_DEF_MissiSecurityCategories, missi.addr());
  if (r != RC_OK) {
    throw std::runtime_error("Could not decode MISSI tag");
  }
  if (missi->present != MissiSecurityCategories_PR_prbacSecurityCategories) {
    throw std::runtime_error("MISSI tag uses unsupported local RBAC policy");
  }
  for (size_t i=0; i!=missi->choice.prbacSecurityCategories.list.count; ++i) {
    NamedTagSet * asnTagSet = missi->choice.prbacSecurityCategories.list.array[i];
    std::string tagSetName = oid2str(&asnTagSet->tagSetName);
    for (size_t tagn=0; tagn!=asnTagSet->securityTags.list.count; ++tagn) {
      SecurityTag * tag = asnTagSet->securityTags.list.array[tagn];
      bool restrictive = false;
      auto tagSet = label.policy().tagSetLookup(tagSetName);
      switch (tag->present) {
        case SecurityTag_PR_enumeratedAttributes: {
          TagType tagType{TagType::enumeratedPermissive};
          if (tagSet->categories(TagType::enumeratedPermissive).empty()) {
            tagType = TagType::enumeratedRestrictive;
          }
          for (size_t catn = 0; catn != tag->choice.enumeratedAttributes.attributeFlags.list.count; ++catn) {
            auto n = tag->choice.enumeratedAttributes.attributeFlags.list.array[catn];
            Lacv catLacv{std::string((char *)n->buf, n->size)};
            auto cat = tagSet->categoryLookup(tagType, catLacv);
            label.addCategory(cat);
          }
        }
              break;
        case SecurityTag_PR_restrictivebitMap:
          restrictive = true;
        case SecurityTag_PR_permissivebitMap: {
          TagType tagType = (restrictive ? TagType::restrictive : TagType::permissive);
          BIT_STRING_t &bits = (restrictive ? tag->choice.restrictivebitMap.attributeFlags
                                            : tag->choice.permissivebitMap.attributeFlags);
          for (size_t byte = 0; byte != bits.size; ++byte) {
            for (int bit = 0; bit != 8; ++bit) {
              if (bits.buf[byte] & (1 << (7 - bit))) {
                Lacv catLacv{(byte * 8) + bit};
                auto cat = tagSet->categoryLookup(tagType, catLacv);
                label.addCategory(cat);
              }
            }
          }
        }
              break;
        case SecurityTag_PR_freeFormField: {
          Asn<TagType7Data> asn_t7(&asn_DEF_TagType7Data);
          ANY_to_type((ANY_t*)&tag->choice.freeFormField, &asn_DEF_TagType7Data, asn_t7.addr());
          TagType tagType = TagType::informative;
          if (asn_t7->present == TagType7Data_PR_bitSetAttributes) {
            BIT_STRING_t &bits = asn_t7->choice.bitSetAttributes;
            for (size_t byte = 0; byte != bits.size; ++byte) {
              for (int bit = 0; bit != 8; ++bit) {
                if (bits.buf[byte] & (1 << (7 - bit))) {
                  Lacv catLacv{(byte * 8) + bit};
                  auto cat = tagSet->categoryLookup(tagType, catLacv);
                  label.addCategory(cat);
                }
              }
            }
          } else {
            for (size_t catn = 0; catn != asn_t7->choice.securityAttributes.list.count; ++catn) {
              auto n = asn_t7->choice.securityAttributes.list.array[catn];
              Lacv catLacv{std::string((char *)n->buf, n->size)};
              auto cat = tagSet->categoryLookup(tagType, catLacv);
              label.addCategory(cat);
            }
          }
        }
      }
    }
  }
}

void Internal::parse_sslp_cat(Clearance & clearance, ANY * any) {
  if (any->size == 0) {
    throw std::runtime_error("SSLPriv tag missing or corrupt");
  }
  Asn<SSLPrivileges> sslp(&asn_DEF_SSLPrivileges);
  auto r = ANY_to_type(any, &asn_DEF_SSLPrivileges, sslp.addr());
  if (r != RC_OK) {
    throw std::runtime_error("Could not decode MISSI tag");
  }
  for (size_t i=0; i!=sslp->list.count; ++i) {
    NamedTagSetPrivilege * asnTagSet = sslp->list.array[i];
    std::string tagSetName = oid2str(&asnTagSet->tagSetName);
    for (size_t tagn=0; tagn!=asnTagSet->securityTagPrivileges.list.count; ++tagn) {
      SecurityTagPrivilege * tag = asnTagSet->securityTagPrivileges.list.array[tagn];
      bool restrictive = false;
      auto tagSet = clearance.policy().tagSetLookup(tagSetName);
      switch (tag->present) {
        case SecurityTagPrivilege_PR_enumeratedAttributes: {
          TagType tagType{TagType::enumeratedPermissive};
          if (tagSet->categories(TagType::enumeratedPermissive).empty()) {
            tagType = TagType::enumeratedRestrictive;
          }
          for (size_t catn = 0; catn != tag->choice.enumeratedAttributes.list.count; ++catn) {
            auto n = tag->choice.enumeratedAttributes.list.array[catn];
            Lacv catLacv{std::string((char *)n->buf, n->size)};
            auto cat = tagSet->categoryLookup(tagType, catLacv);
            clearance.addCategory(cat);
          }
        }
              break;
        case SecurityTagPrivilege_PR_restrictivebitMap:
          restrictive = true;
        case SecurityTagPrivilege_PR_permissivebitMap: {
          TagType tagType = (restrictive ? TagType::restrictive : TagType::permissive);
          BIT_STRING_t &bits = (restrictive ? tag->choice.restrictivebitMap
                                            : tag->choice.permissivebitMap);
          for (size_t byte = 0; byte != bits.size; ++byte) {
            for (int bit = 0; bit != 8; ++bit) {
              if (bits.buf[byte] & (1 << (7 - bit))) {
                Lacv catLacv{(byte * 8) + bit};
                auto cat = tagSet->categoryLookup(tagType, catLacv);
                clearance.addCategory(cat);
              }
            }
          }
        }
              break;
      }
    }
  }
}

