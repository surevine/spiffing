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

#ifndef SPIFFING_CLEARANCE_H
#define SPIFFING_CLEARANCE_H

#include <string>
#include <set>
#include <spiffing/constants.h>
#include <spiffing/categoryref.h>

namespace Spiffing {
	class Clearance {
	public:
		Clearance(std::string const & label, Format fmt);
		void parse(std::string const & label, Format fmt);
		void write(Format fmt, std::string &) const;

		bool hasClassification(lacv_t lacv) const;
		std::set<lacv_t> const & classifications() const {
			return m_classList;
		}
		std::string const & policy_id() const {
			return m_policy_id;
		}
		std::set<CategoryRef> const & categories() const {
			return m_cats;
		}
		bool hasCategory(CategoryRef const & cat) const;
		bool hasCategory(std::set<CategoryRef> const & cats) const;
		Spif const & policy() const {
			return *m_policy;
		}
		void addCategory(std::shared_ptr<Category> const & cat);
	private:
		void parse_xml(std::string const &);
        void parse_xml_debug(std::string const &);
        void parse_xml_nato(std::string const &);
		void parse_ber(std::string const &);
		void parse_any(std::string const &);
        void write_xml_debug(std::string &) const;
        void write_xml_nato(std::string &) const;
		void write_ber(std::string &) const;

		std::shared_ptr<Spif> m_policy;
		std::string m_policy_id;
		std::set<lacv_t> m_classList;
		std::set<CategoryRef> m_cats;
	};
}

#endif
