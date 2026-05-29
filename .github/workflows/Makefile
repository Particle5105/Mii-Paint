TARGET		:= MiiPainterStudio
BLOCKS		:= source

include $(DEVKITARM)/3ds_rules

CFLAGS		:= -g -Wall -O2 -mword-relocations -fomit-frame-pointer -ffast-math
CFLAGS		+= -I$(TARGET) -I$(DEVKITPRO)/libctru/include -I$(DEVKITPRO)/portlibs/3ds/include
CXXFLAGS	:= $(CFLAGS) -fno-rtti -fno-exceptions

LIBS		:= -lcitro2d -lcitro3d -lctru -lm
LIBDIRS		:= $(DEVKITPRO)/libctru $(DEVKITPRO)/portlibs/3ds

export OUTPUT	:= $(CURDIR)/$(TARGET)
export VPATH	:= $(foreach dir,$(BLOCKS),$(CURDIR)/$(dir))
export DEPSDIR	:= $(CURDIR)/build

CFILES		:= $(foreach dir,$(BLOCKS),$(notdir $(wildcard $(dir)/*.c)))
OFILES		:= $(CFILES:.c=.o)
export OBJS	:= $(foreach obj,$(OFILES),$(CURDIR)/build/$(obj))

.PHONY: all clean

all: $(TARGET).3dsx

$(TARGET).3dsx: $(TARGET).elf
$(TARGET).elf: $(OBJS)

clean:
	@rm -rf build $(TARGET).elf $(TARGET).3dsx

ifneq ($(BUILD),build)
$(TARGET).3dsx:
	@mkdir -p build
	@$(MAKE) --no-print-directory -C build -f $(CURDIR)/Makefile BUILD=build

clean:
	@rm -rf build $(TARGET).elf $(TARGET).3dsx
else
$(TARGET).elf: $(OBJS)
	@$(CC) -specs=3dsx.specs $(CFLAGS) $^ $(LIBDIRS:%=-L%/lib) $(LIBS) -o $@

build/%.o: %.c
	@$(CC) -MMD -MP -MF build/$*.d $(CFLAGS) -c $< -o $@
endif
