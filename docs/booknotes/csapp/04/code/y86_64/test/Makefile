YAS_DIR=../assembler/build
YAS=$(YAS_DIR)/yas

YOFILES=abs-asum-cmov.yo abs-asum-jmp.yo asum.yo asumr.yo asumi.yo cjr.yo j-cc.yo poptest.yo pushquestion.yo pushtest.yo prog1.yo prog2.yo prog3.yo prog4.yo prog5.yo prog6.yo prog7.yo prog8.yo prog9.yo prog10.yo ret-hazard.yo

all: $(YOFILES)

%.yo: %.ys
	$(YAS) $<

clean:
	rm -f *.yo
