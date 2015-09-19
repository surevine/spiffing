EXECUTOR?=

INPUTS=$(wildcard food-label*.xml) $(wildcard food-clearance*.xml)
OUTPUTS=$(INPUTS:%.xml=%-out2.ber)

all:
	$(EXECUTOR) ../test tests.xml
	$(MAKE) EXECUTOR="$(EXECUTOR)" $(OUTPUTS)
	rm $(OUTPUTS)

%-out2.ber: %.xml
	$(EXECUTOR) ../transpifferizer food-policy.xml $< $*-out.ber
	$(EXECUTOR) ../transpifferizer food-policy.xml $*-out.ber $*-out.xml
	$(EXECUTOR) ../transpifferizer food-policy.xml $*-out.xml $@
	diff $@ $*-out.ber
	rm $*-out.ber $*-out.xml
