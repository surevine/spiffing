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

#include <Clearance.h>
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
		Clearance_t * asn_clear = 0;
		std::istream * in = &std::cin;
		if (argc >= 1) {
			in = new std::ifstream(argv[1]);
		}
		std::string data{std::istreambuf_iterator<char>(*in), std::istreambuf_iterator<char>()};
		auto rval = ber_decode(0, &asn_DEF_Clearance, (void **)&asn_clear, &data[0], data.size());
		if (rval.code != RC_OK) {
			throw std::runtime_error("Blah");
		}
		if (asn_clear->classList) {
			std::cout << "Classifications: ";
			for (size_t i{0}; i != asn_clear->classList->size; ++i) {
				if (!asn_clear->classList->buf[i]) continue;
				for (int j{0}; j != 8; ++j) {
					if (asn_clear->classList->buf[i] & (1 << (7 - j))) {
						std::cout << (i*8 + j) << " ";
					}
				}
			}
			std::cout << std::endl;
		}
		std::string oid;
		asn_DEF_OBJECT_IDENTIFIER.xer_encoder(&asn_DEF_OBJECT_IDENTIFIER, &asn_clear->policyId, 0, XER_F_BASIC, write_to_string, &oid);
		std::cout << "Security Policy ID: " << oid << std::endl;
	} catch(...) {
		std::cerr << "Dude! Exception!" << std::endl;
	}
}
