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

#include <spiffing/marking.h>

using namespace Spiffing;

namespace {
  const std::string s_blank;
}

Marking::Marking(std::string const & langTag) : m_langTag(langTag) {}

std::string const & Marking::phrase(MarkingCode loc, std::string const & p) const {
  for (auto & i : m_phrase) {
    if (i.first & loc) {
      if (i.first & MarkingCode::noNameDisplay) {
        return s_blank;
      } else if ((i.first & MarkingCode::replacePolicy) || !i.second) {
        return p;
      } else {
        return *(i.second);
      }
    }
  }
  return p;
}

std::string const & Marking::policyPhrase(MarkingCode loc, std::string const & p) const {
  for (auto & i : m_phrase) {
    if (i.first & (loc | MarkingCode::replacePolicy)) {
      if (!i.second) {
        return p;
      } else {
        return *(i.second);
      }
    }
  }
  return p;
}

bool Marking::suppressClassName(MarkingCode loc) const {
  for (auto & i : m_phrase) {
    if (i.first & loc) {
      if (i.first & MarkingCode::suppressClassName) {
        return true;
      } else {
        return false;
      }
    }
  }
  return false;
}

bool Marking::replacePolicy(MarkingCode loc) const {
  for (auto & i : m_phrase) {
    if (i.first & loc) {
      if (i.first & MarkingCode::replacePolicy) {
        return true;
      } else {
        return false;
      }
    }
  }
  return false;
}

void Marking::addPhrase(int loc, std::optional<std::string> const & p) {
  m_phrase.push_back(std::pair<int,std::optional<std::string>>(loc,p));
}
