#include <spiffing/spif.h>
#include <spiffing/label.h>
#include <spiffing/clearance.h>
#include <fstream>
#include <iostream>

using namespace Spiffing;

template<typename T> void transpifferize(Spif & spif, std::string const & ifn, const char * of) {
  std::ifstream label_file(ifn);
  std::string label_str{std::istreambuf_iterator<char>(label_file), std::istreambuf_iterator<char>()};
  T label(spif, label_str, Format::ANY);
  std::cout << "Marking is '" << spif.displayMarking(label) << "'" << std::endl;
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
    Spif spif(spif_file, Format::XML);
    std::cout << "Loaded SPIF " << spif.policy_id() << std::endl;
    if (argc <= 2) return 0;
    try {
      transpifferize<Label>(spif, argv[2], argv[3]);
      return 0;
    } catch (std::runtime_error &e) {
      std::cerr << "Obviously not a label: " << e.what() << std::endl;
    }
    transpifferize<Clearance>(spif, argv[2], argv[3]);
  } catch(std::exception & e) {
    std::cerr << "Dude, exception: " << e.what() << std::endl;
    return 1;
  }
  return 0;
}
