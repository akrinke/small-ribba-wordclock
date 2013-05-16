###############################################################################
# Makefile for the project wordclock
###############################################################################

## General Flags
PROJECT = wordclock
MCU = atmega168
TARGET = wordclock_mega168.elf
CC = avr-gcc

CPP = avr-g++

## Options common to compile, link and assembly rules
COMMON = -mmcu=$(MCU)

## Compile options common for all C compilation units.
CFLAGS = $(COMMON)
CFLAGS += -Wall -gdwarf-2 -std=gnu99         -DF_CPU=8000000UL -Os -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums
CFLAGS += -MD -MP -MT $(*F).o -MF dep/$(@F).d 

## Assembly specific flags
ASMFLAGS = $(COMMON)
ASMFLAGS += $(CFLAGS)
ASMFLAGS += -x assembler-with-cpp -Wa,-gdwarf2

## Linker flags
LDFLAGS = $(COMMON)
LDFLAGS +=  -Wl,-Map=wordclock_mega168.map


## Intel Hex file production flags
HEX_FLASH_FLAGS = -R .eeprom -R .fuse -R .lock -R .signature

HEX_EEPROM_FLAGS = -j .eeprom
HEX_EEPROM_FLAGS += --set-section-flags=.eeprom="alloc,load"
HEX_EEPROM_FLAGS += --change-section-lma .eeprom=0 --no-change-warnings


## Objects that must be built in order to link
OBJECTS = base.o dcf77.o display.o display_tix.o display_wc.o display_wc_eng.o display_wc_ger3.o display_wc_ger.o i2c-master.o i2c-rtc.o irmp.o ldr.o main.o pwm.o shift.o timer.o uart.o user.o wceeprom.o color_effects.o 

## Objects explicitly added by the user
LINKONLYOBJECTS = 

## Build
all: $(TARGET) wordclock_mega168.hex wordclock_mega168.eep wordclock_mega168.lss size

## Compile
base.o: ../src/base.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

dcf77.o: ../src/dcf77.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

display.o: ../src/display.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

display_tix.o: ../src/display_tix.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

display_wc.o: ../src/display_wc.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

display_wc_eng.o: ../src/display_wc_eng.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

display_wc_ger3.o: ../src/display_wc_ger3.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

display_wc_ger.o: ../src/display_wc_ger.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

i2c-master.o: ../src/i2c-master.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

i2c-rtc.o: ../src/i2c-rtc.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

irmp.o: ../src/irmp.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

ldr.o: ../src/ldr.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

main.o: ../src/main.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

pwm.o: ../src/pwm.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

shift.o: ../src/shift.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

timer.o: ../src/timer.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

uart.o: ../src/uart.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

user.o: ../src/user.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

wceeprom.o: ../src/wceeprom.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

color_effects.o: ../src/color_effects.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

##Link
$(TARGET): $(OBJECTS)
	 $(CC) $(LDFLAGS) $(OBJECTS) $(LINKONLYOBJECTS) $(LIBDIRS) $(LIBS) -o $(TARGET)

%.hex: $(TARGET)
	avr-objcopy -O ihex $(HEX_FLASH_FLAGS)  $< $@

%.eep: $(TARGET)
	-avr-objcopy $(HEX_EEPROM_FLAGS) -O ihex $< $@ || exit 0

%.lss: $(TARGET)
	avr-objdump -h -S $< > $@

size: ${TARGET}
	@echo
	@avr-size -C --mcu=${MCU} ${TARGET}

## Clean target
.PHONY: clean
clean:
	-rm -rf $(OBJECTS) wordclock_mega168.elf dep/* wordclock_mega168.hex wordclock_mega168.eep wordclock_mega168.lss wordclock_mega168.map


## Other dependencies
-include $(shell mkdir dep 2>/dev/null) $(wildcard dep/*)

