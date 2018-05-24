# Makefile for Spiffing

SPIFFINGBUILD?=build
W_DIR=$(shell pwd)
ASN1C?=$(W_DIR)/deps/asn1c/asn1c/asn1c -S $(W_DIR)/deps/asn1c/skeletons

all: spifflicator transpifferizer $(SPIFFINGBUILD)/libspiffing.a
	@echo "That's all folks."

help:
	@echo "Sensible targets are:"
	@echo "  all: [default] Build the tools and library"
	@echo "  pre-build: Do everything that needs doing first"
	@echo "  test: Build the test tool"
	@echo "  quick-tests: Run a test sequence"
	@echo "  tests: Run a lengthy tests sequence and build reports"
	@echo "  help: Lists sensible targets"
	@echo "  travis: If you're Travis-CI, run this first"
	@echo "  submodules: Initialise and update submodules"

travis:
	add-apt-repository -y ppa:ubuntu-toolchain-r/test
	apt-get update -qq
	apt-get install -qq gcc-5 g++-5
	update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-5 90
	update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-5 90
	pip install cpp-coveralls
	g++ --version

pre-build: submodules

submodules:
	@[ -d deps/asn1c/asn1c ] || git submodule update --init

$(W_DIR)/deps/asn1c/asn1c/asn1c: submodules
	cd deps/asn1c && autoreconf -iv
	cd deps/asn1c && ./configure --prefix=$(W_DIR)/tmp-asn1c
	cd deps/asn1c && make

asn1c: $(W_DIR)/deps/asn1c/asn1c/asn1c
	@echo "ASN1 Compiler Ready."

tests:
	@echo "Rebuilding:: Clean"
	@$(MAKE) clean
	@echo "Rebuilding:: ASN.1"
	@$(MAKE) gen-ber/.marker
	@echo "Rebuilding:: ASN.1 Compile"
	@$(MAKE) -j6 "DEBUG=-g --coverage" build/libspiffing-asn.a
	@echo "Rebuilding:: Rest"
	@$(MAKE) -j6 "MAKELEVEL=0" "DEBUG=-g --coverage" test-spiffing spifflicator transpifferizer
	@echo "Valgrind test"
	@$(MAKE) -C test-data/ EXECUTOR="valgrind --error-exitcode=99 --leak-check=full --show-leak-kinds=all"
	@echo "Coverage test"
	@$(MAKE) coverage
	@echo "CLang test"
	@scan-build -o report/clang make SPIFFINGBUILD=tmp-analyzer tmp-analyzer/libspiffing.a
	@rm -rf tmp-analyzer

quick-tests: test-spiffing transpifferizer
	@$(MAKE) -C test-data/

coverage: quick-tests
	@$(MAKE) -C test-data/
	@lcov --base-directory . --directory build/ --capture --output-file build/spiffing.info
	@lcov --remove build/spiffing.info deps/*
	@mkdir -p report/lcov/
	@genhtml --output-directory report/lcov/ build/spiffing.info

ASNSOURCE=ESSSecurityLabel.asn Clearance.asn SSLPrivileges.asn MissiSecurityCategories.asn
GENBERSOURCE=$(wildcard gen-ber/*.c) $(ASNSOURCE:%.asn=gen-ber/%.c)
GENBEROBJS=$(GENBERSOURCE:.c=.o)
SPIFFINGSOURCE=$(wildcard src/*.cc)
SPIFFINGOBJS=$(SPIFFINGSOURCE:src/%.cc=$(SPIFFINGBUILD)/spiffing/%.o)

DEBUG?=-g# --coverage #-fprofile-dir=./build/ #-fprofile-generate=./build/ #-DEMIT_ASN_DEBUG=1
CXXFLAGS=-std=c++14

gen-ber/.marker: $(ASNSOURCE) acp145.asn
	@mkdir -p $(dir $@)
	@touch gen-ber/.marker
	@(cd $(dir $@) && $(ASN1C) -fwide-types -fcompound-names $(^:%=../%))
	@-mv gen-ber/converter-example.c .
	@echo $(GENBEROBJS) $(GENBERSOURCE)

gen-ber/%.c gen-ber/%.h: gen-ber/.marker $(ASN1C)
	@echo "ASN.1 Parsing"

converter-sample.c: gen-ber/.marker

.PRECIOUS: converter-example.c gen-ber/%.c gen-ber/%.h gen-ber/.marker

ifeq (0,$(MAKELEVEL))
build/libspiffing-asn.a: $(GENBEROBJS) gen-ber/.marker
	@$(MAKE) submake
else
build/libspiffing-asn.a: $(GENBEROBJS)
	@mkdir -p $(dir $@)
	@rm -f $@
	@ar rcs $@ $^
	@echo Created and ranlib?
endif

$(SPIFFINGBUILD)/libspiffing.a: $(SPIFFINGOBJS)
	@rm -f $@
	@ar rcs $@ $^

submake: build/libspiffing-asn.a
	@echo Done

gen-ber/%.o: gen-ber/%.c
	@echo [CC PIC] $@
	@$(CC) -fPIC $(DEBUG) -c -o $@ -Igen-ber/ $^

clean:
	@echo [CLEAN]
	@rm -rf build/ gen-ber/ report/ spifflicator clearance-parser label-parser clearance-reader label-reader converter-example.c

build/label-parser.o: converter-sample.c build/libspiffing-asn.a
	@echo [CC] $@
	@$(CC) $(DEBUG) -c -o $@ -Igen-ber/ -DPDU=ESSSecurityLabel converter-sample.c

label-parser: build/label-parser.o build/libspiffing-asn.a
	@echo [LINK] $@
	@$(CC) $(DEBUG) -o $@ -Lbuild/ build/label-parser.o -lspiffing-asn


build/clearance-parser.o: converter-sample.c build/libspiffing-asn.a
	@echo [CC] $@
	@$(CC) $(DEBUG) -c -o $@ -Igen-ber/ -DPDU=Clearance converter-sample.c

clearance-parser: build/clearance-parser.o build/libspiffing-asn.a
	@echo [LINK] $@
	@$(CC) $(DEBUG) -o $@ -Lbuild/ $< -lspiffing-asn

$(SPIFFINGBUILD)/spiffing/%.o: src/%.cc gen-ber/ANY.h
	@echo [C++ PIC] $@
	@mkdir -p $(dir $@)
	@$(CXX) -fPIC $(DEBUG) $(CXXFLAGS) -Iinclude/ -Igen-ber/ -Ideps/rapidxml/ -o $@ -MD -MF $(@:%.o=%.d) -c $<

$(SPIFFINGBUILD)/%.o: %.cc
	@echo [C++] $@
	@mkdir -p $(dir $@)
	@$(CXX) $(DEBUG) $(CXXFLAGS) -Iinclude/ -Igen-ber/ -Ideps/rapidxml/ -o $@ -MD -MF $(@:%.o=%.d) -c $<

#
# Junk targets : Examples and what-not.
#

label-reader: build/label-reader.o build/libspiffing-asn.a
	@echo [LINK] $@
	@$(CXX) $(DEBUG) $(CXXFLAGS) -Lbuild/ -o $@ $< -lspiffing-asn

clearance-reader: build/clearance-reader.o build/libspiffing-asn.a
	@echo [LINK] $@
	@$(CXX) $(DEBUG) $(CXXFLAGS) -Lbuild/ -o $@ $< -lspiffing-asn

spifflicator: build/spifflicator.o build/libspiffing.a build/libspiffing-asn.a
	@echo [LINK] $@
	@$(CXX) $(DEBUG) $(CXXFLAGS) -L$(SPIFFINGBUILD) -Lbuild/ -o $@ $< -lspiffing -lspiffing-asn

transpifferizer: build/transpifferizer.o build/libspiffing.a build/libspiffing-asn.a
	@echo [LINK] $@
	@$(CXX) $(DEBUG) $(CXXFLAGS) -L$(SPIFFINGBUILD) -Lbuild/ -o $@ $< -lspiffing -lspiffing-asn

test-spiffing: build/test.o build/libspiffing.a build/libspiffing-asn.a
	@echo [LINK] $@
	@$(CXX) $(DEBUG) $(CXXFLAGS) -L$(SPIFFINGBUILD) -Lbuild/ -o $@ $< -lspiffing -lspiffing-asn

-include build/spiffing/*.d
-include build/*.d
