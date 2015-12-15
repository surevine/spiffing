EXECUTOR?=

INPUTS=$(wildcard food-label*.xml) $(wildcard food-clearance*.xml)
OUTPUTS=$(INPUTS:%.xml=%-nato-out2.ber) $(INPUTS:%.xml=%-missi-out2.ber)

all:
	$(EXECUTOR) ../test tests.xml
	$(MAKE) EXECUTOR="$(EXECUTOR)" $(OUTPUTS)
	rm $(OUTPUTS)

food-label-%-nato-out2.ber: food-label-%.xml
	$(EXECUTOR) ../transpifferizer -p food-policy.xml $< $*-nato-out.ber
	$(EXECUTOR) ../transpifferizer -p food-policy.xml $*-nato-out.ber $*-nato-out.xml
	$(EXECUTOR) ../transpifferizer -p food-policy.xml $*-nato-out.xml $@
	diff $@ $*-nato-out.ber
	rm $*-nato-out.ber $*-nato-out.xml

food-label-%-missi-out2.ber: food-label-%.xml
	$(EXECUTOR) ../transpifferizer -p food-policy-missi.xml $< $*-missi-out.ber
	$(EXECUTOR) ../transpifferizer -p food-policy-missi.xml $*-missi-out.ber $*-missi-out.xml
	$(EXECUTOR) ../transpifferizer -p food-policy-missi.xml $*-missi-out.xml $@
	diff $@ $*-missi-out.ber
	rm $*-missi-out.ber $*-missi-out.xml

food-clearance-%-nato-out2.ber: food-clearance-%.xml
	$(EXECUTOR) ../transpifferizer -p food-policy.xml -c $< $*-nato-out.ber
	$(EXECUTOR) ../transpifferizer -p food-policy.xml -c $*-nato-out.ber $*-nato-out.xml
	$(EXECUTOR) ../transpifferizer -p food-policy.xml -c $*-nato-out.xml $@
	diff $@ $*-nato-out.ber
	rm $*-nato-out.ber $*-nato-out.xml

food-clearance-%-missi-out2.ber: food-clearance-%.xml
	$(EXECUTOR) ../transpifferizer -p food-policy-missi.xml -c $< $*-missi-out.ber
	$(EXECUTOR) ../transpifferizer -p food-policy-missi.xml -c $*-missi-out.ber $*-missi-out.xml
	$(EXECUTOR) ../transpifferizer -p food-policy-missi.xml -c $*-missi-out.xml $@
	diff $@ $*-missi-out.ber
	rm $*-missi-out.ber $*-missi-out.xml
