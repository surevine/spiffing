#include <spiffing/spif.h>
#include <spiffing/label.h>
#include <spiffing/clearance.h>
#include <fstream>
#include <iostream>

int main(int argc, char *argv[]) {
	using namespace Spiffing;
	try {
		if (argc <= 1) {
			std::cout << "spifflicator spif.xml [label.ber [clearance.ber]]" << std::endl;
			return 0;
		}
		std::ifstream spif_file(argv[1]);
		Spif spif(spif_file, Format::XML);
		std::cout << "Loaded SPIF " << spif.policy_id() << std::endl;
		if (argc <= 2) return 0;
		std::ifstream label_file(argv[2]);
		std::string label_str{std::istreambuf_iterator<char>(label_file), std::istreambuf_iterator<char>()};
		Label label(spif, label_str, Format::ANY);
		std::cout << "Label marking is '" << spif.displayMarking(label) << "'" << std::endl;
		if (argc <= 3) return 0;
		std::ifstream clearance_file(argv[3]);
		std::string clearance_str{std::istreambuf_iterator<char>(clearance_file), std::istreambuf_iterator<char>()};
		Clearance clearance(spif, clearance_str, Format::ANY);
		std::cout << "Clearance marking is '" << spif.displayMarking(clearance) << "'" << std::endl;
		std::cout << "ACDF decision is " << (spif.acdf(label, clearance) ? "PASS" : "FAIL") << std::endl;
	} catch(std::exception & e) {
		std::cerr << "Dude, exception: " << e.what() << std::endl;
		return 1;
	}
	return 0;
}
