
PROJ = kswitch

SUBDIRS += src

.PHONY: all clean flash start serial fw

ifneq ($(RULES_MK),y)

export ROOT := $(CURDIR)

# Make firmware for all numbers of kickstart images, pack into a zip file
TARGETS = $(foreach x, 2 3 4 5 6 7 8, $(PROJ)_$(x))
fw: $(TARGETS)
	rm -f fw.zip
	zip -r fw fw
$(PROJ)_%:
	$(MAKE) -f $(ROOT)/Rules.mk clean
	CFLAGS=-DNR_KICK_IMAGES=$* $(MAKE) all
	mkdir -p fw
	mv src/$(PROJ).hex fw/$@.hex

all:
	$(MAKE) -C src -f $(ROOT)/Rules.mk $(PROJ).elf $(PROJ).bin $(PROJ).hex

clean:
	rm -rf fw fw.zip
	$(MAKE) -f $(ROOT)/Rules.mk $@

endif

flash: all
	sudo ~/stm32flash/stm32flash -vw src/$(PROJ).hex /dev/ttyUSB0

start:
	sudo ~/stm32flash/stm32flash -g 0 /dev/ttyUSB0

serial:
	sudo miniterm.py /dev/ttyUSB0 57600
