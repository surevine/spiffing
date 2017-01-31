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

#include <spiffing/clearance.h>
#include <spiffing/lacv.h>
#include <spiffing/spif.h>
#include <spiffing/tagset.h>
#include <spiffing/tag.h>
#include <spiffing/catutils.h>
#include <Clearance.h>
#include <RestrictiveTag.h>
#include <PermissiveTag.h>
#include <EnumeratedTag.h>
#include <stdexcept>
#include <rapidxml.hpp>
#include <rapidxml_print.hpp>
#include <sstream>
#include <spiffing/spiffing.h>

Spiffing::Clearance::Clearance(std::string const & clearance, Spiffing::Format fmt)
: m_policy() {
	parse(clearance, fmt);
}

void Spiffing::Clearance::addCategory(std::shared_ptr<Category> const & cat) {
	m_cats.insert(cat);
}

void Spiffing::Clearance::parse(std::string const & clearance, Spiffing::Format fmt) {
	if (clearance.empty()) {
		throw std::runtime_error("No data to parse");
	}
	switch (fmt) {
	case Spiffing::Format::BER:
		parse_ber(clearance);
		break;
	case Spiffing::Format::XML:
		parse_xml(clearance);
		break;
	case Spiffing::Format::ANY:
		parse_any(clearance);
		break;
	default:
		throw std::runtime_error("Unknown format");
	}
}

void Spiffing::Clearance::write(Spiffing::Format fmt, std::string & output) const {
	switch (fmt) {
	case Spiffing::Format::BER:
		write_ber(output);
		break;
	case Spiffing::Format::XML:
		write_xml_debug(output);
		break;
    case Spiffing::Format::NATO:
        write_xml_nato(output);
        break;
	case Spiffing::Format::ANY:
		throw std::runtime_error("Unknown format");
	}
}

void Spiffing::Clearance::parse_ber(std::string const & clearance) {
	Asn<Clearance_t> asn_clearance(&asn_DEF_Clearance);
	auto rval = ber_decode(0, &asn_DEF_Clearance, asn_clearance.addr(), clearance.data(), clearance.size());
	if (rval.code != RC_OK) {
		throw std::runtime_error("Failed to parse BER/DER encoded clearance");
	}
	if (asn_clearance->classList) {
		for (size_t i{0}; i != (size_t)asn_clearance->classList->size; ++i) {
			if (!asn_clearance->classList->buf[i]) continue;
			for (int j{0}; j != 8; ++j) {
				if (asn_clearance->classList->buf[i] & (1 << (7 - j))) {
					m_classList.insert(i*8 + j);
				}
			}
		}
	}
	m_policy_id = Spiffing::Internal::oid2str(&asn_clearance->policyId);
	m_policy = Site::site().spif(m_policy_id);
	if (asn_clearance->securityCategories) {
		for (size_t i{0}; i != (size_t)asn_clearance->securityCategories->list.count; ++i) {
			std::string tagType = Spiffing::Internal::oid2str(&asn_clearance->securityCategories->list.array[i]->type);
			if (tagType == OID::NATO_EnumeratedPermissive) { // Enum permissive.
				Spiffing::Internal::parse_enum_cat<Clearance>(TagType::enumeratedPermissive, *this, &asn_clearance->securityCategories->list.array[i]->value);
			} else if (tagType == OID::NATO_EnumeratedRestrictive) { // Enum restrictive.
				Spiffing::Internal::parse_enum_cat<Clearance>(TagType::enumeratedRestrictive, *this, 	&asn_clearance->securityCategories->list.array[i]->value);
			} else if (tagType == OID::NATO_RestrictiveBitmap) { // Restrictive.
				Spiffing::Internal::parse_cat<Clearance, RestrictiveTag_t>(TagType::restrictive, &asn_DEF_RestrictiveTag, *this, &asn_clearance->securityCategories->list.array[i]->value);
			} else if (tagType == OID::NATO_PermissiveBitmap) { // Permissive.
				Spiffing::Internal::parse_cat<Clearance, PermissiveTag_t>(TagType::permissive, &asn_DEF_PermissiveTag, *this, &asn_clearance->securityCategories->list.array[i]->value);
			} else if (tagType == OID::SSLPrivilege) { // SSLPriv
				Spiffing::Internal::parse_sslp_cat(*this, &asn_clearance->securityCategories->list.array[i]->value);
			}
		}
	}
}

bool Spiffing::Clearance::hasClassification(lacv_t lacv) const {
	return m_classList.find(lacv) != m_classList.end();
}

bool Spiffing::Clearance::hasCategory(CategoryRef const & cat) const {
	return m_cats.find(cat) != m_cats.end();
}

bool Spiffing::Clearance::hasCategory(std::set<CategoryRef> const & cats) const {
	for (auto & cat : cats) {
		if (hasCategory(cat)) return true;
	}
	return false;
}

void Spiffing::Clearance::parse_xml_debug(std::string const & clearance) {
	using namespace rapidxml;
	std::string tmp{clearance};
	xml_document<> doc;
	doc.parse<0>(const_cast<char *>(tmp.c_str()));
	auto root = doc.first_node();
	if (std::string("clearance") != root->name()) {
		throw std::runtime_error("Not a clearance");
	}
	if (root->xmlns() == nullptr ||
		std::string("http://surevine.com/xmlns/spiffy") != root->xmlns()) {
		throw std::runtime_error("XML Namespace of clearance is unknown");
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
	} else throw std::runtime_error("No policy in clearance");
	// Get classification.
	for (auto securityClassification = root->first_node("classification"); securityClassification; securityClassification = securityClassification->next_sibling("classification")) {
		auto lacvattr = securityClassification->first_attribute("lacv");
		if (lacvattr) {
			lacv_t lacv = std::stoull(lacvattr->value());
			m_classList.insert(lacv);
		}
	}
	// Find tagsets.
	for (auto tag = root->first_node("tag"); tag; tag = tag->next_sibling("tag")) {
		auto typeattr = tag->first_attribute("type");
		if (!typeattr) throw std::runtime_error("tag without type");
		std::string tag_type = typeattr->value();
		Spiffing::TagType type;
		if (tag_type == "restrictive") {
			type = Spiffing::TagType::restrictive;
		} else if (tag_type == "permissive") {
			type = Spiffing::TagType::permissive;
		} else if (tag_type == "enumeratedPermissive") {
			type = Spiffing::TagType::enumeratedPermissive;
		} else if (tag_type == "enumeratedRestrictive") {
			type = Spiffing::TagType::enumeratedRestrictive;
		} else throw std::runtime_error("unsupported tag type " + tag_type);
		auto idattr = tag->first_attribute("id");
		if (!idattr) throw std::runtime_error("tag without id");
		std::string id = idattr->value();
		auto lacvattr = tag->first_attribute("lacv");
		if (!lacvattr) throw std::runtime_error("tag without lacv");
		Spiffing::Lacv lacv = Spiffing::Lacv::parse(std::string(lacvattr->value(), lacvattr->value_size()));
		auto cat = m_policy->tagSetLookup(id)->categoryLookup(type, lacv);
		addCategory(cat);
	}
}


void Spiffing::Clearance::parse_xml(std::string const & label) {
	using namespace rapidxml;
	std::string tmp{label};
	xml_document<> doc;
	doc.parse<parse_fastest>(const_cast<char *>(tmp.c_str()));
	auto root = doc.first_node();
	if (root->xmlns() == nullptr) {
		throw std::runtime_error("XML Namespace of label is unknown");
	}
	std::string xmlns{root->xmlns(), root->xmlns_size()};
	if (xmlns == "http://surevine.com/xmlns/spiffy") {
		parse_xml_debug(label);
	} else if (xmlns == "urn:nato:stanag:4774:confidentialityclearance:1:0") {
		parse_xml_nato(label);
	} else {
		throw std::runtime_error("Namespace not found");
	}
}

void Spiffing::Clearance::parse_xml_nato(std::string const & clearance) {
	using namespace rapidxml;
	std::string tmp{clearance};
	char const * slab{"urn:nato:stanag:4774:confidentialitymetadatalabel:1:0"};
	xml_document<> doc;
	doc.parse<0>(const_cast<char *>(tmp.c_str()));
	auto root = doc.first_node();
	if (std::string("ConfidentialityClearance") != root->name()) {
		throw std::runtime_error("Not a clearance");
	}
	if (root->xmlns() == nullptr ||
		std::string("urn:nato:stanag:4774:confidentialityclearance:1:0") != root->xmlns()) {
		throw std::runtime_error("XML Namespace of clearance is unknown");
	}
	// Get security policy.
	auto securityPolicy = root->first_node("PolicyIdentifier", slab);
	if (securityPolicy) {
		auto idattr = securityPolicy->first_attribute("URI");
		if (idattr) m_policy_id = idattr->value();
		if (m_policy_id.find("urn:oid:") == 0) {
			m_policy_id = m_policy_id.substr(8);
		} else {
			m_policy_id = "";
		}
		m_policy = Site::site().spif_by_name(securityPolicy->value());
		if (m_policy_id.empty()) m_policy_id = m_policy->policy_id();
		if (m_policy->policy_id() != m_policy_id) {
			throw std::runtime_error("Policy mismatch: " + m_policy_id);
		}
	} else throw std::runtime_error("No policy in clearance");
	// Get classification list.
	auto classList = root->first_node("ClassificationList");
	for(auto securityClassification = classList->first_node("Classification", slab); securityClassification; securityClassification = securityClassification->next_sibling("Classification", slab)) {
		auto classn = m_policy->classificationLookup(securityClassification->value());
		if (classn) {
			m_classList.insert(classn->lacv());
		}
	}
	// Find tagsets.
	for (auto tag = root->first_node("Category", slab); tag; tag = tag->next_sibling("Category", slab)) {
		auto typeattr = tag->first_attribute("Type");
		if (!typeattr) throw std::runtime_error("tag without Type");
		std::string tag_type = typeattr->value();
		TagType type;
		if (tag_type == "RESTRICTIVE") {
			type = TagType::restrictive;
		} else if (tag_type == "PERMISSIVE") {
			type = TagType::permissive;
		} else throw std::runtime_error("unsupported tag type " + tag_type);
		auto tagname_a = tag->first_attribute("TagName");
		if (!tagname_a) throw std::runtime_error("Category without TagName");
		std::string tagname = tagname_a->value();
		auto tagSet = m_policy->tagSetLookupByName(tagname);
		for (auto valtag = tag->first_node("GenericValue"); valtag; valtag = valtag->next_sibling("GenericValue")) {
			auto cat = tagSet->categoryLookup(type, valtag->value());
			addCategory(cat);
		}
	}
}

void Spiffing::Clearance::parse_any(std::string const & clearance) {
	try {
		rapidxml::xml_document<> doc;
		doc.parse<rapidxml::parse_fastest>(const_cast<char *>(clearance.c_str()));
	} catch (rapidxml::parse_error & e) {
		parse_ber(clearance);
		return;
	}
	parse_xml(clearance);
}

void Spiffing::Clearance::write_xml_debug(std::string & output) const {
	using namespace rapidxml;
	// Do something sensible.
	xml_document<> doc;
	auto root = doc.allocate_node(node_element, "clearance");
	root->append_attribute(doc.allocate_attribute("xmlns", "http://surevine.com/xmlns/spiffy"));
	auto policy = doc.allocate_node(node_element, "policy");
	policy->append_attribute(doc.allocate_attribute("id", m_policy_id.c_str()));
	root->append_node(policy);
	for (auto const & lacv : m_classList) {
		auto classn = doc.allocate_node(node_element, "classification");
		std::ostringstream ss;
		ss << lacv;
		char * p = doc.allocate_string(ss.str().c_str());
		classn->append_attribute(doc.allocate_attribute("lacv", p));
		root->append_node(classn);
	}
	// Category Encoding
	for (auto const & cat : m_cats) {
		auto tag = doc.allocate_node(node_element, "tag");
		const char * p = nullptr;
		switch (cat->tag().tagType()) {
		case Spiffing::TagType::restrictive:
			p = "restrictive";
			break;
		case Spiffing::TagType::permissive:
			p = "permissive";
			break;
		case Spiffing::TagType::enumeratedPermissive:
			p = "enumeratedPermissive";
			break;
		case Spiffing::TagType::enumeratedRestrictive:
			p = "enumeratedRestrictive";
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


void Spiffing::Clearance::write_xml_nato(std::string & output) const {
	using namespace rapidxml;
	const char * slab = "urn:nato:stanag:4774:confidentialitymetadatalabel:1:0";
	// Do something sensible.
	xml_document<> doc;
	auto root = doc.allocate_node(node_element, "ConfidentialityClearance");
	root->append_attribute(doc.allocate_attribute("xmlns", "urn:nato:stanag:4774:confidentialityclearance:1:0"));
	auto policy = doc.allocate_node(node_element, "PolicyIdentifier");
	policy->append_attribute(doc.allocate_attribute("xmlns", slab));
	std::string policy_uri{"urn:oid:" + m_policy_id};
	policy->append_attribute(doc.allocate_attribute("URI", policy_uri.c_str()));
	policy->value(m_policy->name().c_str());
	root->append_node(policy);
	auto classList = doc.allocate_node(node_element, "ClassificationList");
	for (auto lacv : m_classList) {
		auto classn = doc.allocate_node(node_element, "Classification");
		auto cls = m_policy->classificationLookup(lacv);
		classn->append_attribute(doc.allocate_attribute("xmlns", slab));
		classn->value(cls->name().c_str());
		classList->append_node(classn);
	}
	root->append_node(classList);
	// Category Encoding
	std::string tagset_id;
	TagType tagType = TagType::informative; // Dummy
	xml_node<> * tag = nullptr;
	for (auto const & cat : m_cats) {
		if (cat->tag().tagSet().id() != tagset_id
			|| cat->tag().tagType() != tagType) {
			tagset_id = cat->tag().tagSet().id();
			tagType = cat->tag().tagType();
			tag = doc.allocate_node(node_element, "Category");
			tag->append_attribute(doc.allocate_attribute("xmlns", slab));
			const char *p = nullptr;
			switch (cat->tag().tagType()) {
				case TagType::enumeratedRestrictive:
				case TagType::restrictive:
					p = "RESTRICTIVE";
					break;
				case TagType::enumeratedPermissive:
				case TagType::permissive:
					p = "PERMISSIVE";
					break;
				default:
					throw std::runtime_error("Tagtype unimplemented!");
			}
			tag->append_attribute(doc.allocate_attribute("Type", p));
			// Note: needs a temp string.
			// tag->append_attribute(doc.allocate_attribute("URI", "urn:oid:" + cat->tag().tagSet().id().c_str()));
			tag->append_attribute(doc.allocate_attribute("TagName", cat->tag().tagSet().name().c_str()));
			root->append_node(tag);
		}
		auto catval = doc.allocate_node(node_element, "GenericValue");
		catval->value(cat->name().c_str());
		assert(tag);
		tag->append_node(catval);
	}
    doc.append_node(root);
	rapidxml::print(std::back_inserter(output), doc, rapidxml::print_no_indenting);
}


void Spiffing::Clearance::write_ber(std::string & output) const {
	Asn<Clearance_t> asn_clearance(&asn_DEF_Clearance);
	asn_clearance.alloc();
	Spiffing::Internal::str2oid(m_policy_id, &asn_clearance->policyId);
	Asn<ClassList_t> asn_classlist(&asn_DEF_ClassList);
	asn_classlist.alloc();
	asn_clearance->classList = asn_classlist.release();
	std::size_t sz = 4;
	asn_clearance->classList->buf = (unsigned char *)calloc(sizeof(unsigned char), sz);
	asn_clearance->classList->size = 0;
	for (auto const & cls : m_classList) {
		if (sz <= (cls / 8)) {
			std::size_t old_sz = sz;
			while (sz <= (cls / 8)) sz += 4;
			asn_clearance->classList->buf = (unsigned char *)realloc(asn_clearance->classList->buf, sz);
			memset(asn_clearance->classList->buf + old_sz, 0, sz - old_sz);
		}
		asn_clearance->classList->buf[cls / 8] |= (1 << (7 - (cls % 8)));
		asn_clearance->classList->size = (cls / 8) + 1;
	}
	// Category Encoding.
	if (m_policy->privilegeId() == OID::SSLPrivilege) {
		asn_clearance->securityCategories = Internal::sslp_catencode(m_cats);
	} else {
		asn_clearance->securityCategories = Internal::nato_catencode(m_cats);
	}
	// Actual encoding.
	der_encode(&asn_DEF_Clearance, &*asn_clearance, Spiffing::Internal::write_to_string, &output);
}
