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
#include <string>
#include <set>
#include <memory>

namespace Spiffing {
	class Classification {
	public:
		Classification(lacv_t lacv, std::string const & name, unsigned long hierarchy, bool obsolete=false);
		void colour(std::string const &);
		std::string const & color() const;
		bool operator < (Classification const &) const;

		lacv_t lacv() const {
			return m_lacv;
		}
		std::string const & name() const {
			return m_name;
		}
		void addRequiredCategory(std::unique_ptr<CategoryGroup> reqCats);
		void addMarking(std::unique_ptr<Marking> marking);
		void compile(Spif const &);
		bool valid(Label const &) const;
	private:
		lacv_t m_lacv;
		std::string const m_name;
		unsigned long m_hierarchy;
		bool m_obsolete;
		std::set<std::unique_ptr<CategoryGroup>> m_reqCats;
		std::unique_ptr<Marking> m_marking;
	};

}

#endif
