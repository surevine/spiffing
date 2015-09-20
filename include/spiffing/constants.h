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
		DER,		// Equal to BER.
		XER,		// X.693 ; categories don't work here due to ANY DEFINED BY.
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
	class CategoryData;
	class CategoryGroup;

    // OID constants
    namespace OID {
        // RBAC / Privilege identifiers
        std::string const NATO{"2.16.840.1.101.2.1.8.3"}; // ACP-145(A)
        std::string const MISSI{"2.16.840.1.101.2.1.8.1"};
        std::string const SSLPrivilege{"2.16.840.1.101.2.1.8.2"};

        // ACP-145(A) types
        std::string const NATO_RestrictiveBitmap{"2.16.840.1.101.2.1.8.3.0"};
        std::string const NATO_PermissiveBitmap{"2.16.840.1.101.2.1.8.3.2"};
        std::string const NATO_EnumeratedPermissive{"2.16.840.1.101.2.1.8.3.1"};
        std::string const NATO_EnumeratedRestrictive{"2.16.840.1.101.2.1.8.3.4"};
        std::string const NATO_Informative{"2.16.840.1.101.2.1.8.3.3"};
    }
}

#endif
