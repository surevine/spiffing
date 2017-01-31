EXECUTOR?=

INPUTS=$(wildcard food-label*.xml) $(wildcard food-clearance*.xml)
NATO_INPUTS=$(wildcard nato-clearance*.nato)
OUTPUTS=$(INPUTS:%.xml=%-nato-out2.ber) $(INPUTS:%.xml=%-nato-out2.nato) $(INPUTS:%.xml=%-missi-out2.ber)
OUTPUTS+= $(NATO_INPUTS:%.nato=%-nato-out2.ber)

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

food-label-%-nato-out2.nato: food-label-%.xml
	$(EXECUTOR) ../transpifferizer -p food-policy.xml $< $*-nato-out.nato
	$(EXECUTOR) ../transpifferizer -p food-policy.xml $*-nato-out.nato $*-nato-out.xml
	$(EXECUTOR) ../transpifferizer -p food-policy.xml $*-nato-out.xml $@
	diff $@ $*-nato-out.nato
	rm $*-nato-out.nato $*-nato-out.xml

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

food-clearance-%-nato-out2.nato: food-clearance-%.xml
	$(EXECUTOR) ../transpifferizer -p food-policy.xml -c $< $*-nato-out.nato
	$(EXECUTOR) ../transpifferizer -p food-policy.xml -c $*-nato-out.nato $*-nato-out.xml
	$(EXECUTOR) ../transpifferizer -p food-policy.xml -c $*-nato-out.xml $@
	diff $@ $*-nato-out.nato
	rm $*-nato-out.nato $*-nato-out.xml

nato-clearance-%-nato-out2.ber: nato-clearance-%.nato
	$(EXECUTOR) ../transpifferizer -p nato-4774-policy.xml -c $< $*-nato-out.ber
	$(EXECUTOR) ../transpifferizer -p nato-4774-policy.xml -c $*-nato-out.ber $*-nato-out.xml
	$(EXECUTOR) ../transpifferizer -p nato-4774-policy.xml -c $*-nato-out.xml $*-nato-out.nato
	$(EXECUTOR) ../transpifferizer -p nato-4774-policy.xml -c $*-nato-out.nato $@
	diff $@ $*-nato-out.ber
	rm $*-nato-out.nato $*-nato-out.ber $*-nato-out.xml

food-clearance-%-missi-out2.ber: food-clearance-%.xml
	$(EXECUTOR) ../transpifferizer -p food-policy-missi.xml -c $< $*-missi-out.ber
	$(EXECUTOR) ../transpifferizer -p food-policy-missi.xml -c $*-missi-out.ber $*-missi-out.xml
	$(EXECUTOR) ../transpifferizer -p food-policy-missi.xml -c $*-missi-out.xml $@
	diff $@ $*-missi-out.ber
	rm $*-missi-out.ber $*-missi-out.xml
