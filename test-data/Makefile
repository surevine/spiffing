EXECUTOR?=

INPUTS=$(wildcard food-label*.xml) $(wildcard food-clearance*.xml)
OUTPUTS=$(INPUTS:%.xml=%-nato-out2.ber) $(INPUTS:%.xml=%-missi-out2.ber)

all:
	$(EXECUTOR) ../test tests.xml
	$(MAKE) EXECUTOR="$(EXECUTOR)" $(OUTPUTS)
	rm $(OUTPUTS)

%-nato-out2.ber: %.xml
	$(EXECUTOR) ../transpifferizer food-policy.xml $< $*-nato-out.ber
	$(EXECUTOR) ../transpifferizer food-policy.xml $*-nato-out.ber $*-nato-out.xml
	$(EXECUTOR) ../transpifferizer food-policy.xml $*-nato-out.xml $@
	diff $@ $*-nato-out.ber
	rm $*-nato-out.ber $*-nato-out.xml

%-missi-out2.ber: %.xml
	$(EXECUTOR) ../transpifferizer food-policy-missi.xml $< $*-missi-out.ber
	$(EXECUTOR) ../transpifferizer food-policy-missi.xml $*-missi-out.ber $*-missi-out.xml
	$(EXECUTOR) ../transpifferizer food-policy-missi.xml $*-missi-out.xml $@
	diff $@ $*-missi-out.ber
	rm $*-missi-out.ber $*-missi-out.xml
