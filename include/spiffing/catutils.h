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

#ifndef SPIFFING_CATUTILS_H
#define SPIFFING_CATUTILS_H

#include <spiffing/constants.h>
#include <spiffing/category.h>
#include <spiffing/categoryref.h>
#include <string>
#include <ANY.h>
#include <OBJECT_IDENTIFIER.h>
#include <EnumeratedTag.h>
#include <InformativeTag.h>
#include <SecurityCategories.h>
#include <stdexcept>
#include <set>

namespace Spiffing {



  template<typename ASN_t> class Asn {
  public:
    Asn(asn_TYPE_descriptor_t * t) : m_def(t), m_obj(nullptr) {};
    ASN_t & operator*() { return *m_obj; }
    ASN_t const & operator*() const { return *m_obj; }
    ASN_t * operator -> () { return m_obj; }
    ASN_t const * operator -> () const { return m_obj; }
    void ** addr() { return reinterpret_cast<void **>(&m_obj); }
    ~Asn() { if (m_obj) m_def->free_struct(m_def, m_obj, 0); }

    void alloc() { m_obj = (ASN_t*)calloc(1, sizeof(ASN_t)); }
    ASN_t * release() {
      ASN_t * tmp{m_obj};
      m_obj = nullptr;
      return tmp;
    }
  private:
    asn_TYPE_descriptor_t * const m_def;
    ASN_t * m_obj;
  };

  namespace Internal {

    int write_to_string(const void * buffer, size_t size, void * app_key);

    std::string oid2str(OBJECT_IDENTIFIER_t * oid);
    OBJECT_IDENTIFIER_t * str2oid(std::string const & s);
    void str2oid(std::string const & s, OBJECT_IDENTIFIER_t *);

    SecurityCategories *nato_catencode(std::set<CategoryRef> const &);
    SecurityCategories *missi_catencode(std::set<CategoryRef> const &);
    SecurityCategories *sslp_catencode(std::set<CategoryRef> const &);

    template<typename Object>
    void parse_enum_cat(TagType enum_type, Object & object, ANY * any) {
      if (any->size == 0) {
        throw std::runtime_error("EnumeratedTag missing or corrupt");
      }
      Asn<EnumeratedTag_t> enumeratedTag(&asn_DEF_EnumeratedTag);
      auto r = ANY_to_type(any, &asn_DEF_EnumeratedTag, enumeratedTag.addr());
      if (r != RC_OK) {
        throw std::runtime_error("Failed to decode enumerated tag");
      }
      std::string tagSetName = oid2str(&enumeratedTag->tagName);
      for (size_t ii{0}; ii != (size_t)enumeratedTag->attributeList.list.count; ++ii) {
        auto n = enumeratedTag->attributeList.list.array[ii];
        Lacv catLacv{std::string((char *)n->buf, n->size)};
        auto cat = object.policy().tagSetLookup(tagSetName)->categoryLookup(enum_type, catLacv);
        object.addCategory(cat);
      }
    }

    template<typename Object, typename Tag>
    void parse_cat(TagType type, asn_TYPE_descriptor_t * asn_def, Object & object, ANY * any) {
      if (any->size == 0) {
        throw std::runtime_error("attributeFlagTag missing or corrupt");
      }
      Asn<Tag> tag(asn_def);
      auto r = ANY_to_type(any, asn_def, tag.addr());
      if (r != RC_OK) {
        throw std::runtime_error("Failed to decode attributeFlag tag");
      }
      std::string tagSetName = oid2str(&tag->tagName);
      for (size_t ii{0}; ii != (size_t)tag->attributeFlags.size; ++ii) {
        if (!tag->attributeFlags.buf[ii]) continue;
        for (int ij{0}; ij != 8; ++ij) {
          if (tag->attributeFlags.buf[ii] & (1 << (7 - ij))) {
            Lacv catLacv{(ii*8 + ij)};
            auto cat = object.policy().tagSetLookup(tagSetName)->categoryLookup(type, catLacv);
            object.addCategory(cat);
          }
        }
      }
    }

    template<typename Object>
    void parse_info_cat(Object & object, ANY * any) {
      if (any->size == 0) {
        throw std::runtime_error("InformativeTag missing or corrupt");
      }
      Asn<InformativeTag> tag(&asn_DEF_InformativeTag);
      auto r = ANY_to_type(any, &asn_DEF_InformativeTag, tag.addr());
      if (r != RC_OK) {
        throw std::runtime_error("Failed to decode informative tag");
      }
      std::string tagSetName = oid2str(&tag->tagName);
      if (tag->field.present == FreeFormField_PR_securityAttributes) {
        for (size_t ii{0}; ii != (size_t)tag->field.choice.securityAttributes.list.count; ++ii) {
          auto n = tag->field.choice.securityAttributes.list.array[ii];
          Lacv catLacv{std::string((char *)n->buf, n->size)};
          auto cat = object.policy().tagSetLookup(tagSetName)->categoryLookup(TagType::informative, catLacv);
          object.addCategory(cat);
        }
      } else {
        for (size_t ii{0}; ii != (size_t)tag->field.choice.bitSetAttributes.size; ++ii) {
          if (!tag->field.choice.bitSetAttributes.buf[ii]) continue;
          for (int ij{0}; ij != 8; ++ij) {
            if (tag->field.choice.bitSetAttributes.buf[ii] & (1 << (7 - ij))) {
              Lacv catLacv{(ii*8 + ij)};
              auto cat = object.policy().tagSetLookup(tagSetName)->categoryLookup(TagType::informative, catLacv);
              object.addCategory(cat);
            }
          }
        }
      }
    }

    void parse_missi_cat(Label & label, ANY * any);
    void parse_sslp_cat(Clearance & clearance, ANY * any);
  }

}

#endif
