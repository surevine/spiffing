#ifndef SPIFFING_SPIF_H
#define SPIFFING_SPIF_H

#include <string>
#include <map>
#include <istream>
#include <spiffing/constants.h>
#include <spiffing/classification.h>
#include <spiffing/marking.h>
#include <memory>

namespace Spiffing {
	class Spif {
	public:
		Spif();
		Spif(std::string const &, Format fmt);
		Spif(std::string &&, Format fmt);
		Spif(std::istream &, Format fmt);

		void parse(std::string const &, Format fmt);

		bool acdf(Label const &, Clearance const &) const;
		std::string displayMarking(Label const &) const;
		std::string displayMarking(Clearance const &) const;

		std::string const & policy_id() const {
			return m_oid;
		}

		std::shared_ptr<TagSet> const & tagSetLookup(std::string const &) const;
		std::shared_ptr<Classification> const & classificationLookup(lacv_t cls) const {
			auto i = m_classifications.find(cls);
			if (i == m_classifications.end()) throw std::runtime_error("Unknown classification");
			return (*i).second;
		}

	private:
		std::string m_name;
		std::string m_oid;
		std::map<lacv_t,std::shared_ptr<Classification>> m_classifications;
		std::map<std::string /* OID */, std::shared_ptr<TagSet>> m_tagSets;
		std::unique_ptr<Marking> m_marking;
	};
}

#endif
