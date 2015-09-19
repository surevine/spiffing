//
// Created by dwd on 03/09/15.
//

#include <spiffing/spiffing.h>

using namespace Spiffing;

namespace {
    Site * s_spiffing = nullptr;
}

Site::Site() : m_spifs() {
    s_spiffing = this;
}

std::shared_ptr<Spif> const & Site::spif(std::string const & oid) const {
    auto i = m_spifs.find(oid);
    if (i == m_spifs.end()) throw std::runtime_error("Unknown policy: " + oid);
    return (*i).second;
}

std::shared_ptr<Spif> Site::load(std::istream & file) {
    std::shared_ptr<Spif> spif(new Spif(file, Format::XML));
    m_spifs[spif->policy_id()] = spif;
    return spif;
}

Site & Spiffing::Site::site() {
    return *s_spiffing;
}