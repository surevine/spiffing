EXECUTOR?=

all:
	$(EXECUTOR) ../test tests.xml
	$(EXECUTOR) ../transpifferizer food-policy.xml food-label-milk-chocolate.xml food-label-milk-chocolate-out.ber
	$(EXECUTOR) ../transpifferizer food-policy.xml food-label-milk-chocolate-out.ber food-label-milk-chocolate-out.xml
	$(EXECUTOR) ../transpifferizer food-policy.xml food-label-milk-chocolate-out.xml food-label-milk-chocolate-out2.ber
	diff food-label-milk-chocolate-out2.ber food-label-milk-chocolate-out.ber
	$(EXECUTOR) ../transpifferizer food-policy.xml food-clearance-lactose-intolerant.xml food-clearance-lactose-intolerant-out.ber
	$(EXECUTOR) ../transpifferizer food-policy.xml food-clearance-lactose-intolerant-out.ber food-clearance-lactose-intolerant-out.xml
	$(EXECUTOR) ../transpifferizer food-policy.xml food-clearance-lactose-intolerant-out.xml food-clearance-lactose-intolerant-out2.ber
	diff food-clearance-lactose-intolerant-out.ber food-clearance-lactose-intolerant-out2.ber
