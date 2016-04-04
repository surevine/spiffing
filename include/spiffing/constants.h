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

#ifndef SPIFFING_CONST_H
#define SPIFFING_CONST_H
#include <string>

namespace Spiffing {
	enum class Format {
		BER,		// X.841 syntax.
		DER = BER,		// Equal to BER.
		XML,		// Namespaced XML.
		ANY			// Try to guess.
	};
  enum class TagType {
      restrictive,
      enumeratedPermissive,
      enumeratedRestrictive,
      permissive,
      informative
  };
	enum class InformativeEncoding {
		notApplicable,
		enumerated,
		bitSet
	};
	enum class EnumeratedEncoding {
		notApplicable,
		permissive,
		restrictive
	};
	typedef unsigned long lacv_t;
	enum class OperationType {
		onlyOne,
		oneOrMore,
		all
	};
	typedef enum MarkingCode {
		pageTop = 1 << 0,
		pageBottom = 1 << 1,
		pageTopBottom = 1 << 2,
		documentEnd = 1 << 3,
		noNameDisplay = 1 << 4,
		noMarkingDisplay = 1 << 5,
		documentStart = 1 << 6,
		suppressClassName = 1 << 7
	} MarkingCode;
	class Classification;
	class Label;
	class Clearance;
	class Spif;
	class TagSet;
	class Tag;
	class Category;
	class CategoryRef;
	class Lacv;
	class Marking;
	class Markings;
	class CategoryData;
	class CategoryGroup;
	class EquivClassification;
	class EquivCat;

    // OID constants
    struct OID {
        // RBAC / Privilege identifiers
        static std::string const NATO; // ACP-145(A)
		static std::string const MISSI;
		static std::string const SSLPrivilege;

        // ACP-145(A) types
		static std::string const NATO_RestrictiveBitmap;
		static std::string const NATO_PermissiveBitmap;
		static std::string const NATO_EnumeratedPermissive;
		static std::string const NATO_EnumeratedRestrictive;
		static std::string const NATO_Informative;
    };
}

#endif
