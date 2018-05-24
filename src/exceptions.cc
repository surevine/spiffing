//
// Created by dwd on 22/05/18.
//

#include "spiffing/exceptions.h"

using namespace Spiffing;

#define DEFINE_ERROR(err, parent) \
err::err(const char * what) : parent(what) {} \
err::err(std::string const & what) : parent(what) {}

DEFINE_ERROR(error, std::runtime_error)
DEFINE_ERROR(parse_error, error)
DEFINE_ERROR(spif_error, parse_error)
DEFINE_ERROR(spif_syntax_error, spif_error)
DEFINE_ERROR(spif_ref_error, error)
DEFINE_ERROR(spif_invariant_error, spif_error)
DEFINE_ERROR(policy_mismatch, error)
DEFINE_ERROR(clearance_error, parse_error)
DEFINE_ERROR(label_error, parse_error)
DEFINE_ERROR(equiv_error, label_error)
