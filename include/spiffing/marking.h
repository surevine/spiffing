#ifndef SPIFFING_MARKING_H
#define SPIFFING_MARKING_H

#include <string>

namespace Spiffing {
  class Marking {
  public:
    Marking();

    std::string const & prefix() const {
      return m_prefix;
    }
    std::string const & prefix(std::string const & p) {
      return m_prefix = p;
    }

    std::string const & suffix() const {
      return m_suffix;
    }
    std::string const & suffix(std::string const & s) {
      return m_suffix = s;
    }

    std::string const & phrase() const {
      return m_phrase;
    }
    std::string const & phrase(std::string const & p) {
      return m_phrase = p;
    }

    std::string const & sep() const {
      return m_sep;
    }
    std::string const & sep(std::string const & s) {
      return m_sep = s;
    }

  private:
    std::string m_prefix;
    std::string m_suffix;
    std::string m_phrase;
    std::string m_sep;
  };
}

#endif
