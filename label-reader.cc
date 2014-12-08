#include <ESSSecurityLabel.h>
#include <EnumeratedTag.h>
#include <iostream>
#include <string>
#include <fstream>
#include <stdexcept>

int
write_to_string(const void * buffer, size_t size, void * app_key) {
	std::string * s = reinterpret_cast<std::string *>(app_key);
	s->append(reinterpret_cast<const char *>(buffer), size);
	return 1; // Something non-negative.
}

int
main(int argc, char *argv[]) {
	try {
		ESSSecurityLabel_t * asn_label = 0;
		std::istream * in = &std::cin;
		if (argc >= 1) {
			in = new std::ifstream(argv[1]);
		}
		std::string data{std::istreambuf_iterator<char>(*in), std::istreambuf_iterator<char>()};
		auto rval = ber_decode(0, &asn_DEF_ESSSecurityLabel, (void **)&asn_label, &data[0], data.size());
		if (rval.code != RC_OK) {
			throw std::runtime_error("Blah");
		}
		if (asn_label->security_classification) {
			std::cout << "Classification: " << *asn_label->security_classification << std::endl;
		}
		if (asn_label->security_policy_identifier) {
			std::string oid;
			asn_DEF_OBJECT_IDENTIFIER.xer_encoder(&asn_DEF_OBJECT_IDENTIFIER, asn_label->security_policy_identifier, 0, XER_F_BASIC, write_to_string, &oid);
			std::cout << "Security Policy ID: " << oid << std::endl;
		}
		if (asn_label->security_categories) {
			for (size_t i{0}; i != asn_label->security_categories->list.count; ++i) {
				std::string tagType;
				asn_DEF_OBJECT_IDENTIFIER.xer_encoder(&asn_DEF_OBJECT_IDENTIFIER, &asn_label->security_categories->list.array[i]->type, 0, XER_F_BASIC, write_to_string, &tagType);
				std::cout << "Category Type: " << tagType << std::endl;
				if (tagType == "2.16.840.1.101.2.1.8.3.1") {
					EnumeratedTag_t * enumeratedTag = 0;
					auto r = ANY_to_type(&asn_label->security_categories->list.array[i]->value, &asn_DEF_EnumeratedTag, (void **)&enumeratedTag);
					if (r != RC_OK) {
						throw std::runtime_error("Blah2");
					}
					std::string tagName;
					asn_DEF_OBJECT_IDENTIFIER.xer_encoder(&asn_DEF_OBJECT_IDENTIFIER, &enumeratedTag->tagName, 0, XER_F_BASIC, write_to_string, &tagName);
					std::cout << "Category OID: " << tagName << std::endl;
					for (size_t ii{0}; ii != enumeratedTag->attributeList.list.count; ++ii) {
						auto n = enumeratedTag->attributeList.list.array[ii];
						unsigned long lacv;
						if (0 > asn_INTEGER2ulong(enumeratedTag->attributeList.list.array[ii], &lacv)) {
							std::string tagText(reinterpret_cast<char *>(enumeratedTag->attributeList.list.array[ii]->buf), enumeratedTag->attributeList.list.array[ii]->size);
							std::cout << "Category Value: " << tagText << std::endl;
						} else {
							std::cout << "Category Value: " << lacv << std::endl;
						}
					}
				}
			}
		}
	} catch(...) {
		std::cerr << "Dude! Exception!" << std::endl;
	}
}
