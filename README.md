Spiffy
======

The beginning of a rippingly good SPIF/Label/Clearance handling library.

[![Build Status](https://travis-ci.org/surevine/spiffing.svg)](https://travis-ci.org/surevine/spiffing)

[![Coverage Status](https://img.shields.io/coveralls/surevine/spiffing.svg)](https://coveralls.io/r/surevine/spiffing)

It's absolutely Top Hole!

Currently, this is experimental C++11 code, which will generate Display Markings for Labels (and Clearances), and perform an Access Control Decision. It handles ACP-145A categories,
BER/DER labels and clearances (X.841/ESS syntaxes), and the Open XML SPIF. There's
also support for an XML format (primarily for testing).

## TODO

On the list:
* Writing BER labels / clearances.
* Validation according to SPIF
* Moar test (though the spifflicator has high code coverage as-is, and is valgrind checked)
* Equivalent Policy handling
* Likely some parts of Display Marking handling missing.
* Informative tag support.

## Building / Running

__Note:__ This library uses [asn1c](https://github.com/vlm/asn1c) from a dependency. In order to build tis firstly download submodules:

```
git submodule update --init
```

This will also pull down [rapidxml](https://github.com/dwd/rapidxml) for you. Then you'll need to build `asn1c`:

```
cd deps/asn1c
./configure
make
```

`make` to build.

`make spifflicator` will give you a spifflicator to run the thing through
its paces:

>  $ `./spifflicator stupid-spif.xml stupid-label.ber stupid-clearance-all.ber`  
>  Loaded SPIF 1.1  
>  Label marking is 'Stupid Confidential'  
>  Clearance marking is '{Stupid Unclassified|Stupid Confidential}'  
>  ACDF decision is PASS

Or a less favourable result:

>  $ `./spifflicator stupid-spif.xml stupid-label.ber stupid-clearance-none.ber`  
>  Loaded SPIF 1.1  
>  Label marking is 'Stupid Confidential'  
>  Clearance marking is '{unclassified (auto)}'  
>  ACDF decision is FAIL

`make transpifferizer` will give you a transpifferizer, which will transpifferize both labels and
clearances between formats (but generally from BER to XML, or XML to XML):

> $ `./transpifferizer stupid-spif.xml stupid-label.ber stupid-label.xml`  
> Loaded SPIF 1.1  
> Label marking is 'Stupid Confidential'  
> Writing stupid-label.xml as XML.  

## Testing

`make test` will run the tests and static analysis tools. This takes a while; you may need to
adjust paths in the Makefile for it to work. It'll output a number of reports (for coverage
and any bugs) in ./reports/

