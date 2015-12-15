/***

Copyright 2014-2015 Dave Cridland
Copyright 2014-2015 Surevine Ltd

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

#include <spiffing/label.h>
#include <ESSSecurityLabel.h>
#include <stdexcept>
#include <EnumeratedTag.h>
#include <RestrictiveTag.h>
#include <PermissiveTag.h>
#include <spiffing/lacv.h>
#include <spiffing/spif.h>
#include <spiffing/tagset.h>
#include <spiffing/tag.h>
#include <spiffing/catutils.h>
#include <spiffing/spiffing.h>
#include <rapidxml.hpp>
#include <rapidxml_print.hpp>
#include <sstream>

using namespace Spiffing;

Label::Label(std::string const & label, Format fmt) : m_policy(), m_class(0) {
	parse(label, fmt);
}

Label::Label(std::shared_ptr<Spif> const & policy, lacv_t cls) : m_policy(policy), m_policy_id(policy->policy_id()), m_class(0) {
	m_class = m_policy->classificationLookup(cls);
}

void Label::parse(std::string const & label, Format fmt) {
	if (label.empty()) {
		throw std::runtime_error("No data to parse");
	}
	switch (fmt) {
	case Format::BER:
		parse_ber(label);
		break;
  case Format::XML:
    parse_xml(label);
		break;
	case Format::ANY:
		parse_any(label);
		break;
	default:
		throw std::runtime_error("Unkown format");
	}
}

void Label::write(Format fmt, std::string & output) const {
	switch (fmt) {
	case Format::BER:
		write_ber(output);
		break;
	case Format::XML:
		write_xml(output);
		break;
	case Format::ANY:
		throw std::runtime_error("Unkown format");
	}
}

void Label::addCategory(std::shared_ptr<Category> const & cat) {
	m_cats.insert(cat);
}

void Label::parse_ber(std::string const & label) {
	Asn<ESSSecurityLabel_t> asn_label(&asn_DEF_ESSSecurityLabel);
	auto rval = ber_decode(0, &asn_DEF_ESSSecurityLabel, asn_label.addr(), label.data(), label.size());
	if (rval.code != RC_OK) {
		throw std::runtime_error("Failed to parse BER/DER encoded label");
	}
	if (asn_label->security_policy_identifier) {
		m_policy_id = Internal::oid2str(asn_label->security_policy_identifier);
		m_policy = Site::site().spif(m_policy_id);
		if (m_policy->policy_id() != m_policy_id) {
			throw std::runtime_error("Policy mismatch: " + m_policy_id);
		}
	} else throw std::runtime_error("No policy in label");
	if (asn_label->security_classification) {
		m_class = m_policy->classificationLookup(*asn_label->security_classification);
	}
	if (asn_label->security_categories) {
		for (size_t i{0}; i != asn_label->security_categories->list.count; ++i) {
			std::string tagType = Internal::oid2str(&asn_label->security_categories->list.array[i]->type);
			if (tagType == OID::NATO_EnumeratedPermissive) { // Enum permissive.
				Internal::parse_enum_cat<Label>(TagType::enumeratedPermissive, *this, &asn_label->security_categories->list.array[i]->value);
			} else if (tagType == OID::NATO_EnumeratedRestrictive) { // Enum restrictive.
				Internal::parse_enum_cat<Label>(TagType::enumeratedRestrictive, *this, 	&asn_label->security_categories->list.array[i]->value);
			} else if (tagType == OID::NATO_RestrictiveBitmap) { // Restrictive.
				Internal::parse_cat<Label,RestrictiveTag_t>(TagType::restrictive, &asn_DEF_RestrictiveTag, *this, &asn_label->security_categories->list.array[i]->value);
			} else if (tagType == OID::NATO_PermissiveBitmap) { // Permissive.
				Internal::parse_cat<Label,PermissiveTag_t>(TagType::permissive, &asn_DEF_PermissiveTag, *this, &asn_label->security_categories->list.array[i]->value);
			} else if (tagType == OID::NATO_Informative) { // Informative
				Internal::parse_info_cat<Label>(*this, &asn_label->security_categories->list.array[i]->value);
			} else if (tagType == OID::MISSI) { // MISSI
				Internal::parse_missi_cat(*this, &asn_label->security_categories->list.array[i]->value);
			}
		}
	}
}

void Label::parse_xml(std::string const & label) {
    using namespace rapidxml;
    std::string tmp{label};
    xml_document<> doc;
		doc.parse<0>(const_cast<char *>(tmp.c_str()));
    auto root = doc.first_node();
    if (std::string("label") != root->name()) {
        throw std::runtime_error("Not a label");
    }
    if (root->xmlns() == nullptr ||
        std::string("http://surevine.com/xmlns/spiffy") != root->xmlns()) {
        throw std::runtime_error("XML Namespace of label is unknown");
    }
    // Get security policy.
    auto securityPolicy = root->first_node("policy");
    if (securityPolicy) {
      auto idattr = securityPolicy->first_attribute("id");
      if (idattr) m_policy_id = idattr->value();
		m_policy = Site::site().spif(m_policy_id);
		if (m_policy->policy_id() != m_policy_id) {
			throw std::runtime_error("Policy mismatch: " + m_policy_id);
		}
	} else throw std::runtime_error("No policy in label");
	// Get classification.
    auto securityClassification = root->first_node("classification");
    if (securityClassification) {
        auto lacvattr = securityClassification->first_attribute("lacv");
		if (lacvattr) {
			lacv_t lacv = std::stoull(lacvattr->value());
			m_class = m_policy->classificationLookup(lacv);
		}
    }
    // Find tagsets.
    for (auto tag = root->first_node("tag"); tag; tag = tag->next_sibling("tag")) {
        auto typeattr = tag->first_attribute("type");
        if (!typeattr) throw std::runtime_error("tag without type");
        std::string tag_type = typeattr->value();
		TagType type;
		if (tag_type == "restrictive") {
			type = TagType::restrictive;
		} else if (tag_type == "permissive") {
			type = TagType::permissive;
		} else if (tag_type == "enumeratedPermissive") {
			type = TagType::enumeratedPermissive;
		} else if (tag_type == "enumeratedRestrictive") {
			type = TagType::enumeratedRestrictive;
		} else if (tag_type == "informative") {
			type = TagType::informative;
		} else throw std::runtime_error("unsupported tag type " + tag_type);
		auto idattr = tag->first_attribute("id");
		if (!idattr) throw std::runtime_error("tag without id");
		std::string id = idattr->value();
		auto lacvattr = tag->first_attribute("lacv");
		if (!lacvattr) throw std::runtime_error("tag without lacv");
		Lacv lacv = Lacv::parse(std::string(lacvattr->value(), lacvattr->value_size()));
		auto cat = m_policy->tagSetLookup(id)->categoryLookup(type, lacv);
		addCategory(cat);
    }
}

void Label::parse_any(std::string const & label) {
	try {
		rapidxml::xml_document<> doc;
		doc.parse<rapidxml::parse_fastest>(const_cast<char *>(label.c_str()));
	} catch (rapidxml::parse_error & e) {
		parse_ber(label);
		return;
	}
	parse_xml(label);
}

void Label::write_xml(std::string & output) const {
	using namespace rapidxml;
	// Do something sensible.
	xml_document<> doc;
	auto root = doc.allocate_node(node_element, "label");
	root->append_attribute(doc.allocate_attribute("xmlns", "http://surevine.com/xmlns/spiffy"));
	auto policy = doc.allocate_node(node_element, "policy");
	policy->append_attribute(doc.allocate_attribute("id", m_policy_id.c_str()));
	root->append_node(policy);
	auto classn = doc.allocate_node(node_element, "classification");
	std::ostringstream ss;
	ss << m_class->lacv();
	std::string lacvstr = ss.str();
	classn->append_attribute(doc.allocate_attribute("lacv", lacvstr.c_str()));
	root->append_node(classn);
	// Category Encoding
	for (auto const & cat : m_cats) {
		auto tag = doc.allocate_node(node_element, "tag");
		const char * p = nullptr;
		switch (cat->tag().tagType()) {
		case TagType::restrictive:
			p = "restrictive";
			break;
		case TagType::permissive:
			p = "permissive";
			break;
		case TagType::enumeratedPermissive:
			p = "enumeratedPermissive";
			break;
		case TagType::enumeratedRestrictive:
			p = "enumeratedRestrictive";
			break;
		case TagType::informative:
			p = "informative";
			break;
		default:
			throw std::runtime_error("Tagtype unimplemented!");
		}
		tag->append_attribute(doc.allocate_attribute("type", p));
		tag->append_attribute(doc.allocate_attribute("lacv", cat->lacv().toString().c_str()));
		tag->append_attribute(doc.allocate_attribute("id", cat->tag().tagSet().id().c_str()));
		root->append_node(tag);
	}
	// Actual encoding
	doc.append_node(root);
	rapidxml::print(std::back_inserter(output), doc, rapidxml::print_no_indenting);
}

void Label::write_ber(std::string & output) const {
	Asn<ESSSecurityLabel_t> asn_label(&asn_DEF_ESSSecurityLabel);
	asn_label.alloc();
	asn_label->security_policy_identifier = Internal::str2oid(m_policy_id);
	asn_label->security_classification = (long *)malloc(sizeof(long));
	*asn_label->security_classification = m_class->lacv();
	// Category Encoding.
	if (m_policy->rbacId() == OID::MISSI) {
		asn_label->security_categories = Internal::missi_catencode(m_cats);
	} else if (m_policy->rbacId() == OID::NATO) {
		asn_label->security_categories = Internal::nato_catencode(m_cats);
	}
	// Actual encoding.
	der_encode(&asn_DEF_ESSSecurityLabel, &*asn_label, Internal::write_to_string, &output);
}

bool Label::hasCategory(CategoryRef const & r) const {
	return m_cats.find(r) != m_cats.end();
}

std::unique_ptr<Label> Label::encrypt(std::string policy_id) const {
	std::unique_ptr<Label> newl = m_class->encrypt(*this, policy_id);
	for (auto & cat : m_cats) {
		cat->encrypt(*newl, policy_id);
	}
	m_policy->encrypt(*newl);
	return newl;
}