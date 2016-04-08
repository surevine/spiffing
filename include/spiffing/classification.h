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

#ifndef SPIFFING_CLASS_H
#define SPIFFING_CLASS_H

#include <spiffing/constants.h>
#include <spiffing/markings.h>
#include <spiffing/equivclass.h>
#include <string>
#include <set>
#include <map>
#include <memory>

namespace Spiffing {
	class Classification {
	public:
		Classification(lacv_t lacv, std::string const & name, unsigned long hierarchy, bool obsolete=false);

		lacv_t lacv() const {
			return m_lacv;
		}
		std::string const & name() const {
			return m_name;
		}
		void addRequiredCategory(std::unique_ptr<CategoryGroup> && reqCats);
		void compile(Spif const &);
		bool valid(Label const &) const;

		bool hasMarkings() const {
			return m_markings != nullptr;
		}
		Markings const & markings() const {
			return *m_markings;
		}
		Markings const & markings(std::unique_ptr<Markings> && m) {
			m_markings = std::move(m);
			return *m_markings;
		}
		std::string const & fgcolour() const {
			return m_fgcolour;
		}
		std::string const & fgcolour(std::string const & c) {
			m_fgcolour = c;
			return m_fgcolour;
		}

		void equivEncrypt(std::shared_ptr<EquivClassification> const & equiv);
		void equivDecrypt(std::shared_ptr<EquivClassification> const & equiv);
		std::unique_ptr<Label> encrypt(Label const &, std::string const &) const;
		std::unique_ptr<Label> decrypt(Label const &) const;

		unsigned long hierarchy() const {
			return m_hierarchy;
		}
	private:
		lacv_t m_lacv;
		std::string const m_name;
		std::string m_fgcolour;
		unsigned long m_hierarchy;
		bool m_obsolete;
		std::set<std::unique_ptr<CategoryGroup>> m_reqCats;
		std::unique_ptr<Markings> m_markings;
		std::map<std::string, std::shared_ptr<EquivClassification>> m_equivEncrypt;
		std::map<std::string, std::shared_ptr<EquivClassification>> m_equivDecrypt;
	};

	struct ClassificationHierarchyCompare {
		bool operator() (std::shared_ptr<Classification> const & l, std::shared_ptr<Classification> const & r) {
			return l->hierarchy() < r->hierarchy();
		}
	};
}

#endif
