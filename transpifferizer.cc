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

using namespace Spiffing;

template<typename T> void transpifferize(std::string const & ifn, const char * of) {
  std::ifstream label_file(ifn);
  std::string label_str{std::istreambuf_iterator<char>(label_file), std::istreambuf_iterator<char>()};
  T label(label_str, Format::ANY);
  std::cout << "Marking is '" << label.policy().displayMarking(label) << "'" << std::endl;
  if (!of) return;
  std::string filename{of};
  std::ofstream output_file(filename);
  std::string::size_type dot = filename.rfind('.');
  Format outformat = Format::BER;
  if (dot != std::string::npos) {
    std::string ext = filename.substr(dot + 1);
    std::cout << "Extension is " << ext << std::endl;
    if (ext == "xml") {
      outformat = Format::XML;
    }
  }
  std::cout << "Writing " << filename;
  switch(outformat) {
  case Format::XML:
    std::cout << " as XML." << std::endl;
    break;
  case Format::BER:
    std::cout << " as DER." << std::endl;
  }
  std::string output;
  label.write(outformat, output);
  output_file << output;
}

int main(int argc, char *argv[]) {
  try {
    if (argc <= 1) {
      std::cout << "transpifferizer spif.xml sioobj.ext [output.ext]" << std::endl;
      std::cout << "    ext can be 'xml', 'ber'." << std::endl;
      std::cout << "    sioobj can be label (tried first), or clearance." << std::endl;
      return 0;
    }
    std::ifstream spif_file(argv[1]);
    Site site;
    auto spif = site.load(spif_file);
    std::cout << "Loaded SPIF " << spif->policy_id() << std::endl;
    if (argc <= 2) return 0;
    try {
      transpifferize<Label>(argv[2], argv[3]);
      return 0;
    } catch (std::runtime_error &e) {
      std::cerr << "Obviously not a label: " << e.what() << std::endl;
    }
    transpifferize<Clearance>(argv[2], argv[3]);
  } catch(std::exception & e) {
    std::cerr << "Dude, exception: " << e.what() << std::endl;
    return 1;
  }
  return 0;
}
