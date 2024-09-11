PLATFORM = none

ifeq ($(strip $(PLATFORM)),none)
    override PLATFORM := x86_64  
endif  

all:
	@$(MAKE) -C Kernel build-$(PLATFORM) PLATFORM=$(PLATFORM)