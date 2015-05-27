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
#include <SecurityCategory.h>
#include <PermissiveTag.h>
#include <RestrictiveTag.h>
#include <INTEGER.h>

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
    std::size_t sz = 4;
    asn_tag->attributeFlags.buf = (unsigned char *)calloc(sizeof(unsigned char), sz);
    while (i != cats.end() && tagSetId == (*i)->tag().tagSet().id() && ttype == (*i)->tag().tagType()) {
      if (sz <= ((*i)->lacv() / 8)) {
        std::size_t old_sz = sz;
        while (sz <= ((*i)->lacv() / 8)) sz += 4;
        asn_tag->attributeFlags.buf = (unsigned char *)realloc(asn_tag->attributeFlags.buf, sz);
        memset(asn_tag->attributeFlags.buf + old_sz, 0, sz - old_sz);
      }
      asn_tag->attributeFlags.buf[(*i)->lacv() / 8] |= (1 << (7 - ((*i)->lacv() % 8)));
      asn_tag->attributeFlags.size = ((*i)->lacv() / 8) + 1;
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
    std::size_t sz = 4;
    asn_tag->attributeList.list.array = (INTEGER_t **)calloc(sizeof(INTEGER_t *), sz);
    while (i != cats.end() && tagSetId == (*i)->tag().tagSet().id() && ttype == (*i)->tag().tagType()) {
      if (sz <= asn_tag->attributeList.list.count) {
        std::size_t old_sz = sz;
        while (sz <= asn_tag->attributeList.list.count) sz += 4;
        asn_tag->attributeList.list.array = (INTEGER_t **)realloc(asn_tag->attributeList.list.array, sz * sizeof(INTEGER_t *));
        memset(asn_tag->attributeList.list.array + old_sz, 0, (sz - old_sz) * sizeof(INTEGER_t *));
      }
      Asn<INTEGER_t> lacv(&asn_DEF_INTEGER);
      lacv.alloc();
      lacv->buf = (unsigned char *)strdup((*i)->lacv().text().c_str());
      lacv->size = (*i)->lacv().text().size();
      asn_tag->attributeList.list.array[asn_tag->attributeList.list.count] = lacv.release();
      ++asn_tag->attributeList.list.count;
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
    Internal::str2oid("2.16.840.1.101.2.1.8.3.3", &asn_cat->type);
    Asn<InformativeTag_t> asn_tag(&asn_DEF_InformativeTag);
    asn_tag.alloc();
    Internal::str2oid(tagSetId, &asn_tag->tagName);
    if (cat->tag().informativeEncoding() == InformativeEncoding::enumerated) {
      std::size_t sz = 4;
      asn_tag->field.choice.securityAttributes.list.array = (INTEGER_t **)calloc(sizeof(INTEGER_t *), sz);
      while (i != cats.end() && tagSetId == (*i)->tag().tagSet().id() && ttype == (*i)->tag().tagType()) {
        if (sz <= asn_tag->field.choice.securityAttributes.list.count) {
          std::size_t old_sz = sz;
          while (sz <= asn_tag->field.choice.securityAttributes.list.count) sz += 4;
          asn_tag->field.choice.securityAttributes.list.array = (INTEGER_t **)realloc(asn_tag->field.choice.securityAttributes.list.array, sz * sizeof(INTEGER_t *));
          memset(asn_tag->field.choice.securityAttributes.list.array + old_sz, 0, (sz - old_sz) * sizeof(INTEGER_t *));
        }
        Asn<INTEGER_t> lacv(&asn_DEF_INTEGER);
        lacv.alloc();
        lacv->buf = (unsigned char *)strdup((*i)->lacv().text().c_str());
        lacv->size = (*i)->lacv().text().size();
        asn_tag->field.choice.securityAttributes.list.array[asn_tag->field.choice.securityAttributes.list.count] = lacv.release();
        ++asn_tag->field.choice.securityAttributes.list.count;
        ++i;
      }
    } else {
      std::size_t sz = 4;
      asn_tag->field.choice.bitSetAttributes.buf = (unsigned char *)calloc(sizeof(unsigned char), sz);
      while (i != cats.end() && tagSetId == (*i)->tag().tagSet().id() && ttype == (*i)->tag().tagType()) {
        if (sz <= ((*i)->lacv() / 8)) {
          std::size_t old_sz = sz;
          while (sz <= ((*i)->lacv() / 8)) sz += 4;
          asn_tag->field.choice.bitSetAttributes.buf = (unsigned char *)realloc(asn_tag->field.choice.bitSetAttributes.buf, sz);
          memset(asn_tag->field.choice.bitSetAttributes.buf + old_sz, 0, sz - old_sz);
        }
        asn_tag->field.choice.bitSetAttributes.buf[(*i)->lacv() / 8] |= (1 << (7 - ((*i)->lacv() % 8)));
        asn_tag->field.choice.bitSetAttributes.size = ((*i)->lacv() / 8) + 1;
        ++i;
      }
    }
    ANY_fromType(&asn_cat->value, &asn_DEF_EnumeratedTag, &*asn_tag);
    asn_cats->list.array[asn_cats->list.count] = asn_cat.release();
    asn_cats->list.count++;
  }
}

SecurityCategories * Internal::catencode(std::set<CategoryRef> const & cats) {
  // Category Encoding.
  Asn<SecurityCategories_t> asn_cats(&asn_DEF_SecurityCategories);
  asn_cats.alloc();
  // Allocating too much memory here, could be more efficient.
  asn_cats->list.array = (SecurityCategory **)calloc(sizeof(SecurityCategory *), cats.size());
  std::string tagSetId;
  TagType ttype;
  auto i = cats.begin();
  while (i != cats.end()) {
    auto const & cat = *i;
    switch (cat->tag().tagType()) {
    case TagType::permissive:
      catencodebits<PermissiveTag_t>(asn_cats, cats, "2.16.840.1.101.2.1.8.3.2", &asn_DEF_PermissiveTag, i);
      break;
    case TagType::restrictive:
      catencodebits<RestrictiveTag_t>(asn_cats, cats, "2.16.840.1.101.2.1.8.3.0", &asn_DEF_RestrictiveTag, i);
      break;
    case TagType::enumeratedPermissive:
      catencodeenum(asn_cats, cats, "2.16.840.1.101.2.1.8.3.1", i);
      break;
    case TagType::enumeratedRestrictive:
      catencodeenum(asn_cats, cats, "2.16.840.1.101.2.1.8.3.4", i);
      break;
    case TagType::informative:
      catencodeinform(asn_cats, cats, i);
      break;
    default:
      ++i;
    }
  }
  return asn_cats.release();
}
