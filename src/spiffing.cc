//
// Created by dwd on 03/09/15.
//

#include <spiffing/spiffing.h>
#include <spiffing/exceptions.h>

using namespace Spiffing;

namespace {
    Site * s_spiffing = nullptr;
}

Site::Site() : m_spifs() {
    s_spiffing = this;
}

std::shared_ptr<Spif> const & Site::spif(std::string const & oid) const {
    auto i = m_spifs.find(oid);
    if (i == m_spifs.end()) throw spif_ref_error("Unknown policy id: " + oid);
    return (*i).second;
}

std::shared_ptr<Spif> const & Site::spif_by_name(std::string const & name) const {
    auto i = m_spifnames.find(name);
    if (i == m_spifnames.end()) throw spif_ref_error("Unknown policy name: " + name);
    return (*i).second;
}

std::shared_ptr<Spif> Site::load(std::istream & file) {
    std::shared_ptr<Spif> spif = std::make_shared<Spif>(file, Format::XML);
    m_spifs[spif->policy_id()] = spif;
    m_spifnames[spif->name()] = spif;
    return spif;
}

Site & Spiffing::Site::site() {
    return *s_spiffing;
}