#ifndef SPIFFING_CONST_H
#define SPIFFING_CONST_H

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
      informationalAttributes,
			informationalBitSet
  };
	typedef unsigned long lacv_t;
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
}

#endif
