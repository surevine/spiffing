#ifndef SPIFFING_LACV_H
#define SPIFFING_LACV_H

#include <spiffing/constants.h>
#include <string>

namespace Spiffing {
    class Lacv {
    public:
        Lacv(std::string const &);
        Lacv(std::size_t);

        static Lacv parse(std::string const &);

        std::string const & text() const {
          return m_name;
        }
        bool ascii() const;
        bool operator <(Lacv const & l) const {
          return m_name < l.m_name;
        }

        void genString() const;
        std::string const & toString() const {
          if (m_str.empty()) genString();
          return m_str;
        }

        std::size_t operator % (std::size_t m) const {
          if (!m_sz_valid) genString();
          return m_sz % m;
        }

        std::size_t operator / (std::size_t d) const {
          if (!m_sz_valid) genString();
          return m_sz / d;
        }
    private:
        mutable std::size_t m_sz;
        mutable bool m_sz_valid;
        std::string m_name;
        mutable std::string m_str;
    };
}

#endif
