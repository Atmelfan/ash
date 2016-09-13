CROSS   ?=avr-
CC      :=$(CROSS)gcc
CXX     :=$(CROSS)g++
LD      :=$(CROSS)g++
DEBUG   :=$(CROSS)gdb
SIZE    :=$(CROSS)size
OBJCOPY :=$(CROSS)objcopy
OBJDUMP :=$(CROSS)objdump

TARGET=hexapod
MMCU?=atxmega128a1u

SOURCES=$(wildcard *.cpp) $(wildcard *.c)
INCLUDES=

SETTINGS=settings.h

OBJECTS=$(addprefix build/,$(SOURCES:.cpp=.o))

OBJECTS:=$(addprefix build/,$(SOURCES:.c=.o))

CSTD?=c99
COPT=-Os -fdata-sections -ffunction-sections
CFLAGS=-mmcu=$(MMCU) -std=$(CSTD) $(COPT) -Wall -Wno-strict-aliasing
CFLAGS+=$(addprefix -I,$(INCLUDES))
CFLAGS+=-include "$(SETTINGS)"

CXXSTD?=c++98
CXXOPT=$(COPT) -fno-exceptions -fno-rtti
CXXFLAGS=-mmcu=$(MMCU) -std=$(CXXSTD) $(CXXOPT) -Wall -Wno-strict-aliasing
CXXFLAGS+=$(addprefix -I,$(INCLUDES))
CXXFLAGS+=-include "$(SETTINGS)"

LDFLAGS=-mmcu=$(MMCU) -lm -Wl,--gc-sections -Wl,-Map=build/$(TARGET).map,--cref

.PHONY: all
all: build/$(TARGET).hex 
	$(SIZE) -C --mcu=$(MMCU) build/$(TARGET).elf

debug:
	

install: build/$(TARGET).hex
	avrdude -p x128a1u -P usb -c avrisp2 reset -e -U flash:w:build/$(TARGET).hex:i

clean:
	@rm -f build/*.o build/*.d build/*.lst build/*.map build/*.hex build/*.elf

build/$(TARGET).elf: $(OBJECTS)
	$(LD) $(LDFLAGS) $^ -lm -o $@

build/$(TARGET).hex: build/$(TARGET).elf
	$(OBJCOPY) -O ihex -R .eeprom -R .fuse -R .lock -R .signature $< $@

$build/(TARGET).bin: build/$(TARGET).elf
	$(OBJCOPY) -O binary -R .eeprom -R .fuse -R .lock -R .signature $< $@

build/%.o: %.cpp
	$(CXX) -o $@ $(CXXFLAGS) -MMD -MP -MF $(@:%.o=%.d) $< -c

build/%.o: %.c
	$(CC) -o $@ $(CFLAGS) -MMD -MP -MF $(@:%.o=%.d) $< -c

#$(TARGET).lst: $(TARGET).elf
#	$(OBJDUMP) -h -S $< > $@
#windows: build_win/$(TARGET).exe

#avrdude -p x128a1u -P usb -c avrisp2 reset -U flash:r:flash_160913.bin:r