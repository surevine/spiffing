#include <spiffing/spif.h>
#include <spiffing/label.h>
#include <spiffing/clearance.h>
#include <fstream>
#include <iostream>
#include <rapidxml.hpp>

bool test_step(std::size_t testno, rapidxml::xml_node<> * test) {
	using namespace Spiffing;
	try {
		std::ifstream spiffile(test->first_attribute("spif")->value());
		Spif spif(spiffile, Format::XML);
		std::ifstream label_file(test->first_attribute("label")->value());
		std::string label_str{std::istreambuf_iterator<char>(label_file), std::istreambuf_iterator<char>()};
		Label label(spif, label_str, Format::ANY);
		std::string label_marking = spif.displayMarking(label);
		if (label_marking != test->first_attribute("label-marking")->value()) throw std::runtime_error("Mismatching label marking: " + label_marking);
		std::ifstream clearance_file(test->first_attribute("clearance")->value());
		std::string clearance_str{std::istreambuf_iterator<char>(clearance_file), std::istreambuf_iterator<char>()};
		Clearance clearance(spif, clearance_str, Format::ANY);
		std::string clearance_marking = spif.displayMarking(clearance);
		if (clearance_marking != test->first_attribute("clearance-marking")->value()) throw std::runtime_error("Mismatching clearance marking: " + clearance_marking);
		bool acdf = (test->first_attribute("acdf-result")->value()[0] == '1');
		if (acdf != spif.acdf(label, clearance)) throw std::runtime_error(std::string("ACDF result is unexpected ") + (acdf ? "FAIL" : "PASS"));
		auto ex = test->first_attribute("exception");
		if (ex) {
			std::cout << "Test " << testno << " expected exception: " << ex->value() << std::endl;
			return false;
		}
		return true;
	} catch (std::exception & e) {
		auto ex = test->first_attribute("exception");
		if (ex && std::string(ex->value()) == e.what()) return true; // Expected this error; pass.
		std::cout << "Test " << testno << " failed: " << e.what() << std::endl;
	}
	return false;
}

int main(int argc, char *argv[]) {
	using namespace rapidxml;
	try {
		if (argc <= 1) {
			std::cout << "test tests.xml" << std::endl;
			return 0;
		}
		std::ifstream file(argv[1]);
		std::string str{std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>()};
		xml_document<> doc;
		doc.parse<0>(const_cast<char *>(str.c_str()));
		auto root = doc.first_node();
		std::size_t testno = 0;
		std::size_t passes = 0;
		std::cout << "Tests initiaterizing." << std::endl;
		for (auto test = root->first_node("test"); test; test = test->next_sibling("test")) {
			++testno;
			if (test_step(testno, test)) {
				++passes;
			}
		}
		std::cout << "[" << passes << "/" << testno << "] Tests passed." << std::endl;
	} catch(std::exception & e) {
		std::cerr << "Dude, exception: " << e.what() << std::endl;
		return 1;
	}
	return 0;
}
