#ifndef SPIFFING_CLEARANCE_H
#define SPIFFING_CLEARANCE_H

#include <string>
#include <set>
#include <spiffing/constants.h>
#include <spiffing/categoryref.h>

namespace Spiffing {
	class Clearance {
	public:
		Clearance(Spif const & policy, std::string const & label, Format fmt);
		void parse(std::string const & label, Format fmt);
		void write(Format fmt, std::string &);

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
			return m_policy;
		}
		void addCategory(std::shared_ptr<Category> const & cat);
	private:
		void parse_xml(std::string const &);
		void parse_ber(std::string const &);
		void parse_any(std::string const &);
		void write_xml(std::string &);
		void write_ber(std::string &);

		Spif const & m_policy;
		std::string m_policy_id;
		std::set<lacv_t> m_classList;
		std::set<CategoryRef> m_cats;
	};
}

#endif
