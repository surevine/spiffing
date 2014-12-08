#ifndef SPIFFING_LABEL_H
#define SPIFFING_LABEL_H

#include <string>
#include <spiffing/constants.h>
#include <spiffing/categoryref.h>
#include <memory>
#include <set>

namespace Spiffing {
	class Label {
	public:
		Label(Spif const & spif, std::string const & label, Format fmt);
		void parse(std::string const & label, Format fmt);
		void write(Format fmt, std::string & output);

		std::shared_ptr<Classification> const & classification() const {
			return m_class;
		}
		std::string const & policy_id() const {
			return m_policy_id;
		}
		std::set<CategoryRef> const & categories() const {
			return m_cats;
		}
		Spif const & policy() const {
			return m_policy;
		}

		void addCategory(std::shared_ptr<Category> const & cat);
	private:
		void parse_ber(std::string const & label);
		void parse_xml(std::string const & label);
		void parse_any(std::string const & label);

		void write_ber(std::string &);
		void write_xml(std::string &);

		Spif const & m_policy;
		std::string m_policy_id;
		std::shared_ptr<Classification> m_class;
		std::set<CategoryRef> m_cats;
	};
}

#endif
