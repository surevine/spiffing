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
	apt-get install -qq gcc-4.8 g++-4.8
	update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-4.8 90
	update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-4.8 90
	pip install cpp-coveralls

pre-build: $(firstword $(ASN1C)) submodules

submodules:
	@[ -d deps/asn1c/asn1c ] || git submodule update --init

$(W_DIR)/deps/asn1c/asn1c/asn1c: submodules
	cd deps/asn1c && autoreconf -iv
	cd deps/asn1c && ./configure --prefix=$(W_DIR)/tmp-asn1c
	cd deps/asn1c && make

tests: test spifflicator transpifferizer
	@echo "Valgrind test"
	@$(MAKE) -C test-data/ EXECUTOR="valgrind --error-exitcode=99 --leak-check=full"
	@echo "Coverage test"
	@$(MAKE) -C test-data/
	@lcov --base-directory . --directory build/ --capture --output-file build/spiffing.info
	@lcov --remove build/spiffing.info deps/*
	@mkdir -p report/lcov/
	@genhtml --output-directory report/lcov/ build/spiffing.info
	@echo "CLang test"
	@../llvm/tools/clang/tools/scan-build/scan-build -o report/clang --use-analyzer=../llvm-build/Debug+Asserts/bin/clang make SPIFFINGBUILD=tmp-analyzer tmp-analyzer/libspiffing.a
	@rm -rf tmp-analyzer

quick-tests: test transpifferizer
	@$(MAKE) -C test-data/

GENBERSOURCE=$(wildcard gen-ber/*.c) gen-ber/ESSSecurityLabel.c
GENBEROBJS=$(GENBERSOURCE:.c=.o)
SPIFFINGSOURCE=$(wildcard src/*.cc)
SPIFFINGOBJS=$(SPIFFINGSOURCE:src/%.cc=$(SPIFFINGBUILD)/spiffing/%.o)

DEBUG?=-g --coverage #-fprofile-dir=./build/ #-fprofile-generate=./build/ #-DEMIT_ASN_DEBUG=1
CXXFLAGS=-std=c++11

gen-ber/%.c gen-ber/%.h: ESSSecurityLabel.asn Clearance.asn acp145.asn
	@mkdir -p $(dir $@)
	(cd $(dir $@) && $(ASN1C) -fwide-types $(^:%=../%))
	@mv gen-ber/converter-sample.c .
	@echo $(GENBEROBJS) $(GENBERSOURCE)

converter-sample.c: gen-ber/ESSSecurityLabel.c

.PRECIOUS: converter-sample.c gen-ber/%.c gen-ber/%.h

ifeq (0,$(MAKELEVEL))
build/libspiffing-asn.a: $(GENBEROBJS)
	@$(MAKE) submake
else
build/libspiffing-asn.a: $(GENBEROBJS)
	@mkdir -p $(dir $@)
	@rm -f $@
	@ar rcs $@ $^
endif

$(SPIFFINGBUILD)/libspiffing.a: $(SPIFFINGOBJS)
	@rm -f $@
	@ar rcs $@ $^

submake: build/libspiffing-asn.a
	@echo Done

gen-ber/%.o: gen-ber/%.c
	@echo [CC] $@
	@$(CC) $(DEBUG) -c -o $@ -Igen-ber/ $^

clean:
	@echo [CLEAN]
	@rm -rf build/ gen-ber/ report/ spifflicator clearance-parser label-parser clearance-reader label-reader converter-sample.c

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
	@echo [C++] $@
	@mkdir -p $(dir $@)
	@$(CXX) $(DEBUG) $(CXXFLAGS) -Iinclude/ -Igen-ber/ -Ideps/rapidxml/ -o $@ -MD -MF $(@:%.o=%.d) -c $<

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

test: build/test.o build/libspiffing.a build/libspiffing-asn.a
	@echo [LINK] $@
	@$(CXX) $(DEBUG) $(CXXFLAGS) -L$(SPIFFINGBUILD) -Lbuild/ -o $@ $< -lspiffing -lspiffing-asn

-include build/spiffing/*.d
-include build/*.d
