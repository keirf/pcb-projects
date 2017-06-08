
PROJ = kswitch

SUBDIRS += src

.PHONY: all clean dist flash start serial

ifneq ($(RULES_MK),y)

export ROOT := $(CURDIR)

all:
	$(MAKE) -C src -f $(ROOT)/Rules.mk $(PROJ).elf $(PROJ).bin $(PROJ).hex
clean:
	rm -rf kswitch_v2*
	$(MAKE) -f $(ROOT)/Rules.mk $@

dist: all
	rm -rf kswitch_v2*
	mkdir -p kswitch_v2
	cp -a src/$(PROJ).elf kswitch_v2
	cp -a src/$(PROJ).bin kswitch_v2
	cp -a src/$(PROJ).hex kswitch_v2
	zip -r kswitch_v2 kswitch_v2
	rm -rf kswitch_v2

endif

BAUD=921600

flash: all
	sudo ~/stm32flash/stm32flash -b $(BAUD) \
	-vw src/$(PROJ).hex /dev/ttyUSB0

start:
	sudo ~/stm32flash/stm32flash -b $(BAUD) -g 0 /dev/ttyUSB0

serial:
	sudo miniterm.py /dev/ttyUSB0 115200
