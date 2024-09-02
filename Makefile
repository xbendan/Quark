PLATFORM = none

ifeq ($(strip $(PLATFORM)),none)  
	@echo "Platform set to default (x86_64)"  
    override PLATFORM := x86_64
endif  

all:
	@$(MAKE) -C Kernel build-$(PLATFORM) PLATFORM=$(PLATFORM)