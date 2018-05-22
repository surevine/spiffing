//
// Created by dwd on 22/05/18.
//

#ifndef SPIFFING_EXCEPTIONS_H
#define SPIFFING_EXCEPTIONS_H

#include <stdexcept>
#include <string>

namespace Spiffing {
#define DEFINE_ERROR(err, parent) \
    class err : public parent { \
    public: \
        explicit err(const char *); \
        explicit err(std::string const &); \
    }
    DEFINE_ERROR(error, std::runtime_error);
    DEFINE_ERROR(parse_error, error);
    DEFINE_ERROR(spif_error, parse_error);
    DEFINE_ERROR(spif_syntax_error, spif_error);
    DEFINE_ERROR(spif_ref_error, error);
    DEFINE_ERROR(spif_invariant_error, spif_error);
    DEFINE_ERROR(policy_mismatch, error);
    DEFINE_ERROR(clearance_error, parse_error);
    DEFINE_ERROR(label_error, parse_error);
    DEFINE_ERROR(equiv_error, label_error);
#undef DEFINE_ERROR
}

#endif //SPIFFING_EXCEPTIONS_H
