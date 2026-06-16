#-------------------------------------------------------------------------------
# WiiChat – Makefile
# Requires devkitPPC + libogc (devkitPro)
#-------------------------------------------------------------------------------

#-- Project identity -----------------------------------------------------------
TARGET   := boot
BUILD    := build
SOURCES  := source
INCLUDES := include

#-- App metadata (used by wiiload / HBC meta.xml) ------------------------------
APP_NAME    := WiiChat
APP_VERSION := 0.1-dev1
APP_AUTHOR  := WiiChat Contributors

#-- Compiler flags -------------------------------------------------------------
CFLAGS   := -g -O2 -Wall -Wextra \
            -DAPP_NAME=\"$(APP_NAME)\" \
            -DAPP_VERSION=\"$(APP_VERSION)\"
CXXFLAGS := $(CFLAGS)
LDFLAGS  := -g

#-- Libraries ------------------------------------------------------------------
#   ogc provides GX, VIDEO, PAD, WPAD, etc.
LIBS     := -lwiiuse -lbte -logc -lm

#-------------------------------------------------------------------------------
# devkitPro environment
#-------------------------------------------------------------------------------
ifeq ($(strip $(DEVKITPRO)),)
  $(error "Please set DEVKITPRO in your environment. export DEVKITPRO=/opt/devkitpro")
endif

include $(DEVKITPRO)/wii_rules

#-------------------------------------------------------------------------------
# Build rules (auto-discover sources)
#-------------------------------------------------------------------------------
CFILES   := $(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.c)))
CXXFILES := $(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.cpp)))

export VPATH := $(foreach dir,$(SOURCES),$(CURDIR)/$(dir))
export DEPSDIR := $(CURDIR)/$(BUILD)

OFILES := $(CXXFILES:.cpp=.o) $(CFILES:.c=.o)

export OUTPUT := $(CURDIR)/$(TARGET)
export INCLUDE := $(foreach dir,$(INCLUDES),-I$(CURDIR)/$(dir)) \
                  -I$(CURDIR)/$(BUILD) \
                  -I$(LIBOGC_INC)

export LIBPATHS := -L$(LIBOGC_LIB)

.PHONY: all clean

#-- Default target -------------------------------------------------------------
all: $(BUILD)
	@$(MAKE) --no-print-directory -C $(BUILD) -f $(CURDIR)/Makefile

$(BUILD):
	@mkdir -p $@

#-- Clean target ---------------------------------------------------------------
clean:
	@echo Cleaning ...
	@rm -rf $(BUILD) $(TARGET).elf $(TARGET).dol

#-------------------------------------------------------------------------------
# Build rules executed inside $(BUILD)/
#-------------------------------------------------------------------------------
ifneq ($(notdir $(CURDIR)),$(BUILD))

DEPENDS := $(OFILES:.o=.d)

$(OUTPUT).dol: $(OUTPUT).elf
$(OUTPUT).elf: $(OFILES)

-include $(DEPENDS)

endif