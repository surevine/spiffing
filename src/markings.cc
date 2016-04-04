//
// Created by dwd on 04/04/16.
//

#include <spiffing/markings.h>
#include <spiffing/marking.h>

using namespace Spiffing;

Markings::Markings() {}

Marking const * Markings::marking(std::string const & langTag) const {
    auto i = m_marking.find(langTag);
    if (i == m_marking.end()) {
        auto p = langTag.find_first_of("-_");
        if (p != std::string::npos) {
            std::string lang{langTag, p};
            i = m_marking.find(lang);
        }
    }
    if (i == m_marking.end()) {
        i = m_marking.find("");
    }
    if (i == m_marking.end()) {
        return nullptr;
    }
    return (*i).second.get();
}
void Markings::marking(std::unique_ptr<Marking> && m) {
    m_marking.emplace(std::move(std::make_pair(m->langTag(), std::move(m))));
}
