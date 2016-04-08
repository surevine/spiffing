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

#ifndef SPIFFING_MARKING_H
#define SPIFFING_MARKING_H

#include <string>
#include <list>
#include <spiffing/constants.h>
#include <optional>

namespace Spiffing {
  class Marking {
  public:
    Marking(std::string const & langTag);

    std::string const & langTag() const {
      return m_langTag;
    }

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

    std::string const & phrase(MarkingCode loc, std::string const & p) const;
    void addPhrase(int loc, std::optional<std::string> const & p);
    bool suppressClassName(MarkingCode loc) const;
    bool replacePolicy(MarkingCode loc) const;
    std::string const & policyPhrase(MarkingCode loc, std::string const & p) const;

    std::string const & sep() const {
      return m_sep;
    }
    std::string const & sep(std::string const & s) {
      return m_sep = s;
    }

  private:
    std::string m_langTag;
    std::string m_prefix;
    std::string m_suffix;
    std::string m_sep;
    std::list<std::pair<int,std::optional<std::string>>> m_phrase;
  };
}

#endif
