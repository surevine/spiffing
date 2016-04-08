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

struct Opts {
    Opts(int argc, char *argv[])
            : clearance(false), encrypt(false), input(nullptr), output(nullptr), langTag()
    {
      for (int i = 1; i < argc; ++i) {
        if (argv[i][0] == '-') {
          switch (argv[i][1]) {
            case 'p': {
              ++i;
              std::ifstream spif_file(argv[i]);
              /*
               * You can get at the Spiffing::Site object this way - handy for general access.
               * Note that this will cause undefined behaviour if you don't have one.
               */
              auto spif = Site::site().load(spif_file);
              std::cout << "Loaded SPIF " << spif->name() << std::endl;
            }
                  break;
            case 'l':
              ++i;
              langTag = argv[i];
                  break;
            case 'c':
              clearance = true;
                  break;
            case 'e': {
              encrypt = true;
              ++i;
              policy_out = argv[i];
              /*
               * Accessing a policy directly is done via the Site object as well.
               * Note that the encrypt target policy need to be loaded.
               */
              auto spif = Site::site().spif(policy_out);
              std::cout << "Output policy name: " << spif->name() << std::endl;
              std::cout << "Mode ENCRYPT" << std::endl;
            }
                  break;
          }
        } else {
          if (input == nullptr) {
            input = argv[i];
          } else {
            output = argv[i];
          }
        }
      }
    }

    bool clearance;
    bool encrypt;
    std::string policy_out;
      const char * input;
      const char * output;
    std::string langTag;
};

/*
 * Load and display a Label or Clearance.
 * They both support the same API, although are not related classes.
 */
template<typename T> std::unique_ptr<T> load(Opts & opt, std::string const & ifn) {
  std::ifstream label_file(ifn);
  std::string label_str{std::istreambuf_iterator<char>(label_file), std::istreambuf_iterator<char>()};
  std::unique_ptr<T> label(new T(label_str, Format::ANY));
  std::cout << "Marking in " << opt.langTag << " is '" << label->policy().displayMarking(*label, opt.langTag) << "'" << std::endl;
  return label;
}

/*
 * Writing both objects is also similar. Note that you cannot write as
 * Format::ANY.
 */
template<typename T> void write(T const & label, const char * of) {
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
    break;
  case Format::ANY:
    ;
  }
  std::string output;
  label.write(outformat, output);
  output_file << output;
}

/*
 * Translation is only supported in encrypt mode (ie, from the local policy to the remote.
 * Both source and target policies MUST be loaded.
 *
 * The target policy is referenced by OID, here.
 */
std::unique_ptr<Label> translate(Opts const & opt, std::unique_ptr<Label> const & label) {
  std::unique_ptr<Label> newl = label->encrypt(opt.policy_out);
  std::cout << "Translated label has marking '" << newl->policy().displayMarking(*newl, opt.langTag) << "'" << std::endl;
  return newl;
}

int main(int argc, char *argv[]) {
  try {
    if (argc <= 1) {
      std::cout << "transpifferizer [-l langTag] -p spif.xml [-e policyid] [-c] sioobj.ext  [output.ext]" << std::endl;
      std::cout << "    ext can be 'xml', 'ber'." << std::endl;
      std::cout << "    sioobj can be label or clearance [with -c]." << std::endl;
      std::cout << "    only labels can be translated [with -e]." << std::endl;
      return 0;
    }
    if (argc <= 2) return 0;
    /*
     * This object own the policies for its lifetime.
     */
    Site site;
    Opts opts{argc, argv};

    if (opts.clearance) {
      std::unique_ptr<Clearance> clearance = std::move(load<Clearance>(opts, opts.input));
      write(*clearance, opts.output);
    } else {
      std::unique_ptr<Label> label{load<Label>(opts, opts.input)};
      if (opts.encrypt) {
        label = translate(opts, label);
      }
      write(*label, opts.output);
    }
  } catch(std::exception & e) {
    std::cerr << "Dude, exception: " << e.what() << std::endl;
    return 1;
  }
  return 0;
}
