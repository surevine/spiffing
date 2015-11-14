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

#include <spiffing/spif.h>
#include <rapidxml.hpp>
#include <spiffing/label.h>
#include <spiffing/clearance.h>
#include <spiffing/tagset.h>
#include <spiffing/tag.h>
#include <spiffing/category.h>
#include <spiffing/lacv.h>
#include <INTEGER.h>
#include <spiffing/catutils.h>
#include <spiffing/marking.h>
#include <spiffing/categorydata.h>
#include <spiffing/categorygroup.h>

using namespace Spiffing;

Spif::Spif(std::string const & s, Format fmt)
: m_classifications(), m_rbacId("") {
	parse(s, fmt);
}

Spif::Spif(std::istream & s, Format fmt)
: m_classifications() {
	std::string data{std::istreambuf_iterator<char>(s), std::istreambuf_iterator<char>()};
	parse(data, fmt);
}

namespace {
	Lacv parseLacv(rapidxml::xml_attribute<> * lacv) {
		return Lacv::parse(std::string{(char *)lacv->value(), lacv->value_size()});
	}

	std::unique_ptr<Marking> parseMarking(rapidxml::xml_node<> * holder) {
		std::unique_ptr<Marking> ptr;
		for (auto qual = holder->first_node("markingQualifier"); qual; qual = qual->next_sibling("markingQualifier")) {
			for (auto q = qual->first_node("qualifier"); q; q = q->next_sibling("qualifier")) {
				auto qcode_a = q->first_attribute("qualifierCode");
				auto txt_a = q->first_attribute("markingQualifier");
				if (!txt_a) continue;
				std::string txt(txt_a->value(), txt_a->value_size());
				if (qcode_a) {
					std::string qcode(qcode_a->value(), qcode_a->value_size());
					if (!ptr) ptr = std::unique_ptr<Marking>{new Marking()};
					if (qcode == "prefix") {
						ptr->prefix(txt);
					} else if (qcode == "suffix") {
						ptr->suffix(txt);
					} else if (qcode == "separator") {
						ptr->sep(txt);
					}
				}
			}
		}
		for (auto data = holder->first_node("markingData"); data; data = data->next_sibling("markingData")) {
			auto phrase_a = data->first_attribute("phrase");
			if (!ptr) ptr = std::unique_ptr<Marking>{new Marking()};
			int loc(0);
			std::string phrase;
			if (phrase_a) {
				phrase = std::string(phrase_a->value(), phrase_a->value_size());
			}
			bool locset = false;
			bool unknown = false;
			for (auto code = data->first_node("code"); code; code = code->next_sibling("code")) {
				std::string codename(code->value(), code->value_size());
				if (codename == "noMarkingDisplay") {
					loc |= MarkingCode::noMarkingDisplay;
					locset = true;
				} else if (codename == "noNameDisplay") {
					loc |= MarkingCode::noNameDisplay;
				} else if (codename == "suppressClassName") {
					loc |= MarkingCode::suppressClassName;
				} else if (codename == "pageBottom") {
					loc |= MarkingCode::pageBottom;
					locset = true;
				} else if (codename == "pageTop") {
					loc |= MarkingCode::pageTop;
					locset = true;
				} else if (codename == "pageTopBottom") {
					loc |= MarkingCode::pageTop | MarkingCode::pageBottom;
					locset = true;
				} else {
					unknown = true;
				}
			}
			if (unknown) continue;
			if (!locset) {
				loc |= MarkingCode::pageBottom;
			}
			if (phrase_a) {
				ptr->addPhrase(loc, std::string(phrase_a->value(), phrase_a->value_size()));
			} else {
				ptr->addPhrase(loc, "");
			}
		}
		return ptr;
	}

	TagType parseTagType(rapidxml::xml_node<> * node) {
		auto tagType_a = node->first_attribute("tagType");
		if (!tagType_a) throw std::runtime_error("element has no tagType");
		std::string tagTypeName = tagType_a->value();
		TagType tagType;
		if (tagTypeName == "permissive") {
			tagType = TagType::permissive;
		} else if (tagTypeName == "restrictive") {
			tagType = TagType::restrictive;
		} else if (tagTypeName == "enumerated") {
			auto enumType_a = node->first_attribute("enumType");
			if (!enumType_a) throw std::runtime_error("element has no enumType");
			std::string enumTypeName = enumType_a->value();
			if (enumTypeName == "permissive") {
				tagType = TagType::enumeratedPermissive;
			} else {
				tagType = TagType::enumeratedRestrictive;
			}
		} else {
			tagType = TagType::informative;
		}
		return tagType;
	}

	// For excludedCategory, also used within categoryGroup.
	std::unique_ptr<CategoryData> parseOptionalCategoryData(rapidxml::xml_node<> * node) {
		std::string tagSetRef;
		auto tagSetRef_a = node->first_attribute("tagSetRef");
		if (tagSetRef_a) {
			tagSetRef = tagSetRef_a->value();
		} else {
			throw std::runtime_error("Missing tagSetRef in OptionalCategoryData");
		}
		TagType tagType = parseTagType(node);
		auto lacv_a = node->first_attribute("lacv");
		if (lacv_a) {
			Lacv l{parseLacv(lacv_a)};
			return std::unique_ptr<CategoryData>(new CategoryData(tagSetRef, tagType, l));
		} else {
			return std::unique_ptr<CategoryData>(new CategoryData(tagSetRef, tagType));
		}
	}

	// For requiredCategory
	std::unique_ptr<CategoryGroup> parseCategoryGroup(rapidxml::xml_node<> * node) {
		auto op_a = node->first_attribute("operation");
		std::string opname{op_a->value(), op_a->value_size()};
		OperationType opType{OperationType::onlyOne};
		if (opname == "onlyOne") {
			opType = OperationType::onlyOne;
		} else if (opname == "oneOrMore") {
			opType = OperationType::oneOrMore;
		} else if (opname == "all") {
			opType = OperationType::all;
		}
		std::unique_ptr<CategoryGroup> group(new CategoryGroup(opType));
		for (auto cd = node->first_node("categoryGroup"); cd; cd = cd->next_sibling("categoryGroup")) {
			group->addCategoryData(parseOptionalCategoryData(cd));
		}
		return group;
	}

	template<typename T>
	void parseExcludedClass(rapidxml::xml_node<> * node, T & t, std::map<std::string,std::shared_ptr<Classification>> const & classNames) {
		for (auto excClass = node->first_node("excludedClass"); excClass; excClass = excClass->next_sibling("excludedClass")) {
			if (!excClass->value() || !excClass->value_size()) throw std::runtime_error("Empty excludedClass element");
			std::string className{excClass->value(), excClass->value_size()};
			auto it = classNames.find(className);
			if (it == classNames.end()) throw std::runtime_error("Unknown classification in exclusion");
			t->excluded(*(*it).second);
		}
	}

	std::shared_ptr<Classification> parseClassification(rapidxml::xml_node<> * classification) {
		auto lacv_a = classification->first_attribute("lacv");
		lacv_t lacv{strtoull(lacv_a->value(), NULL, 10)};
		auto name_a = classification->first_attribute("name");
		std::string name{name_a->value()};
		auto hierarchy_a = classification->first_attribute("hierarchy");
		unsigned long hierarchy{strtoul(hierarchy_a->value(), NULL, 10)};
		std::shared_ptr<Classification> cls{new Classification(lacv, name, hierarchy)};
		for (auto reqCat = classification->first_node("requiredCategory"); reqCat; reqCat = reqCat->next_sibling("requiredCategory")) {
			cls->addRequiredCategory(parseCategoryGroup(reqCat));
		}
		cls->marking(parseMarking(classification));
		auto color_a = classification->first_attribute("color");
		if (color_a && color_a->value()) {
			cls->fgcolour(color_a->value());
		}
		return cls;
	}

	std::shared_ptr<TagSet> parseTagSet(rapidxml::xml_node<> * tagSet, size_t & ordinal, std::map<std::string,std::shared_ptr<Classification>> const & classNames) {
		auto id_a = tagSet->first_attribute("id");
		auto name_a = tagSet->first_attribute("name");
		std::shared_ptr<TagSet> ts{new TagSet(id_a->value(), name_a->value())};
		ts->addMarking(parseMarking(tagSet));
		for (auto tag = tagSet->first_node("securityCategoryTag"); tag; tag = tag->next_sibling("securityCategoryTag")) {
			auto tagName_a = tag->first_attribute("name");
			if (!tagName_a) throw std::runtime_error("securityCategoryTag has no name");
			TagType tagType = parseTagType(tag);
			InformativeEncoding t7enc = InformativeEncoding::notApplicable;
			if (tagType == TagType::informative) {
				auto t7type_a = tag->first_attribute("tag7Encoding");
				if (!t7type_a) throw std::runtime_error("element has no tag7Encoding");
				std::string t7type = t7type_a->value();
				if (t7type == "bitSetAttributes") {
					t7enc = InformativeEncoding::bitSet;
				} else {
					t7enc = InformativeEncoding::enumerated;
				}
			}
			std::shared_ptr<Tag> t{new Tag(*ts, tagType, t7enc, tagName_a->value())};
			ts->addTag(t);
			t->marking(parseMarking(tag));
			for (auto cat = tag->first_node("tagCategory"); cat; cat = cat->next_sibling("tagCategory")) {
				Lacv lacv = parseLacv(cat->first_attribute("lacv"));
				std::shared_ptr<Category> c{new Category(*t, cat->first_attribute("name")->value(), lacv, ordinal++)};
				parseExcludedClass(cat, c, classNames);
				t->addCategory(c);
				for (auto reqnode = cat->first_node("requiredCategory"); reqnode; reqnode = reqnode->next_sibling("requiredCategory")) {
					std::unique_ptr<CategoryGroup> req = parseCategoryGroup(reqnode);
					c->required(std::move(req));
				}
				for (auto excnode = cat->first_node("excludedCategory"); excnode; excnode = excnode->next_sibling("excludedCategory")) {
					std::unique_ptr<CategoryData> exc = parseOptionalCategoryData(excnode);
					c->excluded(std::move(exc));
				}
				c->marking(parseMarking(cat));
			}
		}
		return ts;
	}
}

void Spif::parse(std::string const & s, Format fmt) {
	using namespace rapidxml;
	if (s.empty()) {
		throw std::runtime_error("SPIF is empty");
	}
	std::string scratch = s;
	xml_document<> doc;
	doc.parse<0>(const_cast<char *>(scratch.c_str()));
	auto node = doc.first_node();
	if (!node) {
		throw std::runtime_error("SPIF XML parse failure, no root node");
	}
	if (std::string("SPIF") != node->name()) {
		throw std::runtime_error("Not a spif");
	}
	auto rbacId = node->first_attribute("rbacId");
	if (rbacId && rbacId->value()) {
		m_rbacId = rbacId->value();
	} else {
		m_rbacId = OID::NATO;
	}
	auto privilegeId = node->first_attribute("privilegeId");
	if (privilegeId && privilegeId->value()) {
		m_privilegeId = privilegeId->value();
	} else {
		m_privilegeId = OID::NATO;
	}
	auto securityPolicyId = node->first_node("securityPolicyId");
	if (securityPolicyId) {
		auto name = securityPolicyId->first_attribute("name");
		if (name) {
			m_name = name->value();
		}
		auto id = securityPolicyId->first_attribute("id");
		if (id) {
			m_oid = id->value();
		}
	}
	auto securityClassifications = node->first_node("securityClassifications");
	std::map<std::string, std::shared_ptr<Classification>> classNames;
	for (auto classn = securityClassifications->first_node("securityClassification"); classn; classn = classn->next_sibling("securityClassification")) {
		std::shared_ptr<Classification> c = parseClassification(classn);
		auto ins = m_classifications.insert(std::make_pair(c->lacv(), c));
		if (!ins.second) {
			throw std::runtime_error("Duplicate classification " + c->name());
		}
		auto ins2 = classNames.insert(std::make_pair(c->name(), c));
		if (!ins2.second) {
			throw std::runtime_error("Duplicate classification name " + c->name());
		}
	}
	auto securityCategoryTagSets = node->first_node("securityCategoryTagSets");
	if (securityCategoryTagSets) {
		size_t ordinal = 0;
		for (auto tagSet = securityCategoryTagSets->first_node("securityCategoryTagSet"); tagSet; tagSet = tagSet->next_sibling("securityCategoryTagSet")) {
			std::shared_ptr<TagSet> ts = parseTagSet(tagSet, ordinal, classNames);
			m_tagSets.insert(std::make_pair(ts->id(), ts));
			m_tagSetsByName.insert(std::make_pair(ts->name(), ts));
		}
	}
	m_marking = parseMarking(node);
	// Once we've parsed all the tagSets, we need to "compile" the category restrictions.
	for (auto & ts : m_tagSets) {
		ts.second->compile(*this);
	}
	for (auto & cls : m_classifications) {
		cls.second->compile(*this);
	}
}

namespace {
	void categoryMarkings(MarkingCode loc, std::string & marking, std::set<CategoryRef> const & cats, std::string const & sep) {
		std::string tagName;
		std::string tagsep;
		for (auto & i : cats) {
			std::string phrase;
			if (i->hasMarking()) {
				phrase = i->marking().phrase(loc, i->name());
			} else if (i->tag().hasMarking()) {
				phrase = i->tag().marking().phrase(loc, i->name());
			} else {
				phrase = i->name();
			}
			if (phrase.empty()) continue;
			std::string currentTagName = i->tag().name();
			if (tagName != currentTagName) {
				// Entering new tag.
				tagName = currentTagName;
				if (!marking.empty()) marking += sep;
				if (i->tag().hasMarking()) {
					tagsep = i->tag().marking().sep();
					if (tagsep.empty()) tagsep = "/"; // Default;
					marking += i->tag().marking().prefix();
				} else {
					tagsep = "/";
				}
			} else {
				marking += tagsep;
			}
			marking += phrase;
		}
	}
}

std::string Spif::displayMarking(Label const & label, MarkingCode loc) const {
	std::string marking;
	if (label.policy_id() != m_oid) {
		throw std::runtime_error("Label is incorrect policy");
	}
	bool suppressClassName = false;
	for (auto & i : label.categories()) {
		if (i->hasMarking()) {
			if (i->marking().suppressClassName(loc)) {
				suppressClassName = true;
				break;
			}
		}
	}
	std::string sep;
	if (m_marking != nullptr) sep = m_marking->sep();
	if (sep.empty()) sep = " "; // Default.
	if (!suppressClassName) {
		if (label.classification().hasMarking()) {
			marking += label.classification().marking().phrase(loc, label.classification().name());
		} else if (m_marking != nullptr) {
			marking += m_marking->phrase(loc, label.classification().name());
		} else {
			marking += label.classification().name();
		}
	}
	categoryMarkings(loc, marking, label.categories(), sep);
	if (m_marking != nullptr) marking = m_marking->prefix() + marking;
	if (m_marking != nullptr) marking += m_marking->suffix();
	return marking;
}

std::string Spif::displayMarking(Spiffing::Clearance const & clearance) const {
	std::string marking;
	bool any{false};
	if (clearance.policy_id() != m_oid) {
		throw std::runtime_error("Clearance is incorrect policy");
	}
	std::string sep;
	if (m_marking != nullptr) marking += m_marking->prefix();
	if (m_marking != nullptr) sep = m_marking->sep();
	if (sep.empty()) sep = " "; // Default.
	marking += "{";
	for (auto cls : clearance.classifications()) {
		auto clsit = m_classifications.find(cls);
		if (clsit == m_classifications.end()) {
			throw std::runtime_error("No classification (from clearance)");
		}
		if (!any) {
			any = true;
		} else {
			marking += "|";
		}
		marking += (*clsit).second->name();
	}
	if (!any) {
		auto clsit = m_classifications.find(0);
		if (clsit == m_classifications.end()) {
			marking += "unclassified (auto)";
		} else {
			marking += (*clsit).second->name();
		}
	}
	marking += "}";
	categoryMarkings(MarkingCode::pageBottom, marking, clearance.categories(), sep);
	if (m_marking != nullptr) marking += m_marking->suffix();
	return marking;
}

bool Spif::acdf(Label const & label, Spiffing::Clearance const & clearance) const {
	if (clearance.policy_id() != m_oid) {
		throw std::runtime_error("Clearance is incorrect policy");
	}
	if (label.policy_id() != m_oid) {
		throw std::runtime_error("Label is incorrect policy");
	}
	if (!clearance.hasClassification(label.classification().lacv())) return false;
	// Now consider each category in the label.
	std::string permissiveTagName;
	std::set<CategoryRef> cats;
	for (auto & cat : label.categories()) {
		Tag const & tag = cat->tag();
		if (!permissiveTagName.empty() && permissiveTagName != tag.name()) {
			if (!clearance.hasCategory(cats)) return false;
			permissiveTagName.clear();
			cats.clear();
		}
		if (tag.tagType() == TagType::restrictive ||
				tag.tagType() == TagType::enumeratedRestrictive) {
				if (!clearance.hasCategory(cat)) return false;
		} else if (tag.tagType() == TagType::permissive ||
								tag.tagType() == TagType::enumeratedPermissive) {
				cats.insert(cat);
				permissiveTagName = tag.name();
		}
	}
	if (!permissiveTagName.empty()) {
		if (!clearance.hasCategory(cats)) return false;
		permissiveTagName.clear();
		cats.clear();
	}
	return true;
}

std::shared_ptr<TagSet> const & Spif::tagSetLookup(std::string const & tagSetId) const {
	auto i = m_tagSets.find(tagSetId);
	if (i == m_tagSets.end()) throw std::runtime_error("Unknown tagset id: " + tagSetId);
	return (*i).second;
}

std::shared_ptr<TagSet> const & Spif::tagSetLookupByName(std::string const & tagSet) const {
	auto i = m_tagSetsByName.find(tagSet);
	if (i == m_tagSetsByName.end()) throw std::runtime_error("Unknown tagset name: " + tagSet);
	return (*i).second;
}

bool Spif::valid(Label const & label) const {
	if (!label.classification().valid(label)) return false;
	for (auto & cat : label.categories()) {
		if (!cat->valid(label)) return false;
	}
	return true;
}
