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

#include <spiffing/spif.h>
#include <spiffing/label.h>
#include <spiffing/clearance.h>
#include <spiffing/spiffing.h>
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

		/*
		 * You need a Spiffing::Site for the duration of your Spiffing usage.
		 *
		 * This owns the Spifs (policies).
		 */
		Site site;

		/*
		 * Spifs are loaded in from an iostream, in this case a file.
		 * The return value is actually a std::shared_ptr<Spiffing::Spif>
		 */
		auto spif = site.load(spif_file);
		std::cout << "Loaded SPIF " << spif->policy_id() << std::endl;
		if (argc <= 2) return 0;
		std::ifstream label_file(argv[2]);
		std::string label_str{std::istreambuf_iterator<char>(label_file), std::istreambuf_iterator<char>()};

		/*
		 * A label is loaded in from a std::string. It will use the Site to locate the correct Spif.
		 *
		 * You can fix the format for a small improvement in speed.
		 */
		Label label(label_str, Format::ANY);

		/*
		 * Most of the driving functions are nominally within the Spif, such as
		 * the displayMarking here:
		 */
		std::cout << "Label marking is '" << spif->displayMarking(label) << "'" << std::endl;
		/*
		 * But you can access the Spif via the Label easily, shown here with valid():
		 */
		if (!label.policy().valid(label)) std::cout << "Label is not valid!" << std::endl;
		if (argc <= 3) return 0;
		std::ifstream clearance_file(argv[3]);
		std::string clearance_str{std::istreambuf_iterator<char>(clearance_file), std::istreambuf_iterator<char>()};
		Clearance clearance(clearance_str, Format::ANY);
		/*
		 * Clearances also have a displayMarking of sorts, though it's not standardized.
		 */
		std::cout << "Clearance marking is '" << spif->displayMarking(clearance) << "'" << std::endl;
		/*
		 * The ACDF check must operate on labels and clearances from the same policy:
		 */
		std::cout << "ACDF decision is " << (spif->acdf(label, clearance) ? "PASS" : "FAIL") << std::endl;
	} catch(std::exception & e) {
		/*
		 * Exceptions thrown are, currently, all std::runtime_errors.
		 */
		std::cerr << "Dude, exception: " << e.what() << std::endl;
		return 1;
	}
	return 0;
}
