#ifndef SPIFFING_CLASS_H
#define SPIFFING_CLASS_H

#include <spiffing/constants.h>
#include <string>

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
	private:
		lacv_t m_lacv;
		std::string const m_name;
		unsigned long m_hierarchy;
		bool m_obsolete;
	};

}

#endif
