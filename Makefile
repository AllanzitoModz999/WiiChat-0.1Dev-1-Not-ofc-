#-------------------------------------------------------------------------------
# WiiChat – Makefile
# Built against devkitPPC + libogc (devkitPro)
#-------------------------------------------------------------------------------
.SUFFIXES:
#-------------------------------------------------------------------------------
ifeq ($(strip $(DEVKITPPC)),)
$(error "Please set DEVKITPPC in your environment. export DEVKITPPC=<path to>devkitPPC")
endif

include $(DEVKITPPC)/wii_rules

#-------------------------------------------------------------------------------
# Project settings
#-------------------------------------------------------------------------------
TARGET   := boot
BUILD    := build
SOURCES  := source
INCLUDES :=
DATA     :=

#-------------------------------------------------------------------------------
# Code-generation options
#-------------------------------------------------------------------------------
CFLAGS   := -g -O2 -Wall $(MACHDEP) $(INCLUDE)
CXXFLAGS := $(CFLAGS)
LDFLAGS  := -g $(MACHDEP) -Wl,-Map,$(notdir $@).map

#-------------------------------------------------------------------------------
# Libraries
#-------------------------------------------------------------------------------
LIBS     := -lwiiuse -lbte -logc -lm

#-------------------------------------------------------------------------------
# Derived lists
#-------------------------------------------------------------------------------
ifneq ($(BUILD),$(notdir $(CURDIR)))

#-- We are in the project root: recurse into $(BUILD) -------------------------

export OUTPUT  := $(CURDIR)/$(TARGET)
export VPATH   := $(foreach dir,$(SOURCES),$(CURDIR)/$(dir)) \
                  $(foreach dir,$(DATA),$(CURDIR)/$(dir))
export DEPSDIR := $(CURDIR)/$(BUILD)

CFILES   := $(foreach dir,$(SOURCES),$(notdir $(wildcard $(CURDIR)/$(dir)/*.c)))
CXXFILES := $(foreach dir,$(SOURCES),$(notdir $(wildcard $(CURDIR)/$(dir)/*.cpp)))
sFILES   := $(foreach dir,$(SOURCES),$(notdir $(wildcard $(CURDIR)/$(dir)/*.s)))
SFILES   := $(foreach dir,$(SOURCES),$(notdir $(wildcard $(CURDIR)/$(dir)/*.S)))

export OFILES := $(CXXFILES:.cpp=.o) $(CFILES:.c=.o) \
                 $(sFILES:.s=.o) $(SFILES:.S=.o)

export INCLUDE := $(foreach dir,$(INCLUDES),-I$(CURDIR)/$(dir)) \
                  -I$(CURDIR)/$(BUILD) \
                  -I$(LIBOGC_INC)

export LIBPATHS := -L$(LIBOGC_LIB)

.PHONY: all clean

all: $(BUILD)
	@$(MAKE) --no-print-directory -C $(BUILD) -f $(CURDIR)/Makefile

$(BUILD):
	@mkdir -p $@

clean:
	@echo Cleaning ...
	@rm -rf $(BUILD) $(TARGET).elf $(TARGET).dol

else

#-- We are inside $(BUILD): compile & link ------------------------------------

DEPENDS := $(OFILES:.o=.d)

$(OUTPUT).dol: $(OUTPUT).elf
$(OUTPUT).elf: $(OFILES)

-include $(DEPENDS)

endif