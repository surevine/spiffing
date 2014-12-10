# Makefile for Spiffing

SPIFFINGBUILD?=build
WD=$(`pwd`)

all: spifflicator transpifferizer $(SPIFFINGBUILD)/libspiffing.a
	@echo "That's all folks."

tests: test spifflicator transpifferizer
	@echo "Valgrind test"
	@valgrind --error-exitcode=99 --leak-check=full ./test tests.xml
	@echo "Coverage test"
	@./test tests.xml
	@./test tests.xml
	@lcov --base-directory . --directory build/ --capture --output-file build/spiffing.info
	@mkdir -p report/lcov/
	@genhtml --output-directory report/lcov/ build/spiffing.info
	@echo "CLang test"
	@../llvm/tools/clang/tools/scan-build/scan-build -o report/clang --use-analyzer=../llvm-build/Debug+Asserts/bin/clang make SPIFFINGBUILD=tmp-analyzer tmp-analyzer/libspiffing.a
	@rm -rf tmp-analyzer

GENBERSOURCE=$(wildcard gen-ber/*.c) gen-ber/ESSSecurityLabel.c
GENBEROBJS=$(GENBERSOURCE:.c=.o)
SPIFFINGSOURCE=$(wildcard src/*.cc)
SPIFFINGOBJS=$(SPIFFINGSOURCE:src/%.cc=$(SPIFFINGBUILD)/spiffing/%.o)

DEBUG?=-g --coverage #-fprofile-dir=./build/ #-fprofile-generate=./build/ #-DEMIT_ASN_DEBUG=1
CXXFLAGS=-std=c++11

gen-ber/%.c gen-ber/%.h: ESSSecurityLabel.asn Clearance.asn acp145.asn
	@mkdir -p $(dir $@)
	(cd $(dir $@) && $(WD)/deps/asn1c/asn1c -fwide-types $(^:%=../%))
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
