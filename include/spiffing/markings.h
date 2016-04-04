//
// Created by dwd on 04/04/16.
//

#ifndef SPIFFING_MARKINGS_H
#define SPIFFING_MARKINGS_H

#include <spiffing/constants.h>
#include <spiffing/marking.h>
#include <memory>
#include <map>

namespace Spiffing {
    class Markings {
    public:
        Markings();
        Marking const * marking(std::string const & langTag) const;
        void marking(std::unique_ptr<Marking> && m);
    private:
        std::map<std::string, std::unique_ptr<Marking>> m_marking;
    };
}

#endif //SPIFFING_MARKINGS_H
