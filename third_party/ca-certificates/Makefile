SRCS := $(wildcard *.crt)
TARGETS := $(SRCS:.crt=.crt.zlib)

.PHONY: all clean

all: $(TARGETS)

%.crt.zlib: %.crt
	python3 compress.py < $^ > $@

%.crt : %.S
	@echo "Skipping assembly file: $<"

clean:
	-rm -f $(TARGETS)
