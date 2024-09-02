FILENAME = quark
PLATFORM = none
# BUILD_VERSION := $(shell git describe --tags --always --dirty)
BUILD_VERSION = 0.0.1
BUILD_MODE = release
BUILD_NAME = $(FILENAME)-v$(BUILD_VERSION)-$(PLATFORM)-$(BUILD_MODE)
FINAL_PATH ?= /mnt/c/Users/MejiroArdan/Codes/Cxxlang/$(BUILD_NAME).iso
BUILD_PATH = Build/platforms/$(PLATFORM)
DIST_PATH = Dist/$(BUILD_NAME).elf
DIST_IMAGE = Dist/$(BUILD_NAME).iso

CXXFLAGS = \
	-Wall -Wextra -Wno-unused-parameter -Wno-sign-compare -Wno-write-strings \
	-Waddress-of-packed-member \
	-DGLOBAL_API_NAMESPACE \
	-ffreestanding -fno-exceptions -fno-rtti -fno-stack-protector -fno-pic \
	-mno-sse -mno-sse2 -mno-sse3 -mno-sse4 -mno-sse4.1 -mno-sse4.2 \
	-mno-avx -mno-avx2 -mno-mmx \
	-std=c++23 \
	-mcmodel=kernel \
	-mno-red-zone \
	-O0 \
	-g  
LDFLAGS = \
	-nostdlib -z max-page-size=0x1000  

# Source files
DIRS := $(shell find ./Source/$(FILENAME)/ -maxdepth 1 -type d)  
DIRS := $(basename $(patsubst ./%,%,$(DIRS)))  

SOURCE_GENERICS := $(foreach dir, $(DIRS), $(wildcard $(dir)/*.cpp))
GENERIC_SOURCE := $(patsubst Source/%.cpp, Build/%.o, $(SOURCE_GENERICS))
DRIVERS_SOURCE := $(patsubst Source/%.cpp, Build/%.o, $(wildcard Source/platforms/impls/*.cpp))

PLATFORM_SOURCE = $(patsubst Source/%.cpp, Build/%.o, $(wildcard Source/platforms/$(PLATFORM)/*.cpp))
PLATFORM_ASSEMBLY = $(patsubst Source/%.asm, Build/%.o, $(wildcard Source/platforms/$(PLATFORM)/*.asm))

$(GENERIC_SOURCE): Build/%.o: Source/%.cpp
	@mkdir -p $(dir $@)
	@echo "CXX $(patsubst Source/%.cpp, %.cpp, $<)..."
	@$(CXX) $(CXXFLAGS) -I Include -c $< -o $@  

$(DRIVERS_SOURCE): Build/%.o: Source/%.cpp
	@mkdir -p $(dir $@)
	@echo "CXX $(patsubst Source/%.cpp, %.cpp, $<)..."
	@$(CXX) $(CXXFLAGS) -I Include -c $< -o $@  

ifeq ($(strip $(PLATFORM)),none)  
	@echo "Platform set to default (x86_64)"  
    override PLATFORM := x86_64
endif  

check_arguments:
	@echo "Checking arguments..."
	@echo "Filename: $(FILENAME)"
	@echo "Platform: $(PLATFORM)"  
    ifeq ($(strip $(PLATFORM)),x86_64)  
		@echo "Setting compilers to x86_64"  
		$(eval CXX := x86_64-elf-g++)  
		$(eval LD := x86_64-elf-ld)  
		$(eval AS := nasm)  
    else ifeq ($(PLATFORM),risc_v)  
		@echo "Setting compilers to risc_v"
		$(eval CXX := riscv64-unknown-elf-g++)
		$(eval LD := riscv64-unknown-elf-ld)
		$(eval AS := riscv64-unknown-elf-as)
    endif  
	@echo "Build version: $(BUILD_VERSION)"
	@echo "Build mode: $(BUILD_MODE)"
	@echo "Build name: $(BUILD_NAME)"
	@echo "Final output path: $(FINAL_PATH)"
	$(eval BUILD_PATH = Build/platforms/$(PLATFORM))
	$(eval DIST_PATH = Dist/$(BUILD_NAME).elf)
	$(eval DIST_IMAGE = Dist/$(BUILD_NAME).iso)

$(PLATFORM_SOURCE): Build/%.o: Source/%.cpp
	@mkdir -p $(dir $@)
	@echo "CXX $(patsubst Source/%.cpp, %.cpp, $<)..."
	@$(CXX) $(CXXFLAGS) -I Include -c $< -o $@
$(PLATFORM_ASSEMBLY): Build/%.o: Source/%.asm
	@mkdir -p $(dir $@)
	@echo "AS $(patsubst Source/%.asm, %.asm, $<)..."
	@$(AS) -f elf64 $< -o $@

auto: check_arguments
	@$(MAKE) clean
	@$(MAKE) build-$(PLATFORM) -j4 PLATFORM=$(PLATFORM) AS=$(AS) CXX=$(CXX) LD=$(LD)  
	@$(MAKE) run-$(PLATFORM) 

build-x86_64: $(GENERIC_SOURCE) $(DRIVERS_SOURCE) $(PLATFORM_SOURCE) $(PLATFORM_ASSEMBLY)
	@echo "Building x86_64 kernel..."  
	@echo "Making building directory $(BUILD_PATH)."  
	@mkdir -p $(BUILD_PATH)  
	@echo "Making dist directory."  
	@mkdir -p Dist  
	@echo "Linking objects..."  
	@$(LD) $(LDFLAGS) -o $(DIST_PATH) \
		-T Target/LinkerScript-$(PLATFORM).ld \
		$(GENERIC_SOURCE) $(PLATFORM_SOURCE) $(PLATFORM_ASSEMBLY)  
	cp $(DIST_PATH) Target/limine_root/boot/$(FILENAME)-$(PLATFORM)-$(BUILD_MODE).elf  
	xorriso -as mkisofs -b boot/limine/limine-bios-cd.bin \
		-no-emul-boot -boot-load-size 4 -boot-info-table \
		--efi-boot boot/limine/limine-uefi-cd.bin \
		-efi-boot-part --efi-boot-image --protective-msdos-label \
		Target/limine_root -o $(DIST_IMAGE)
	../Thirdparty/limine/limine bios-install $(DIST_IMAGE)  
	cp $(DIST_IMAGE) $(FINAL_PATH)  

run-x86_64:
	@echo "Running x86_64 kernel..."
	@qemu-system-x86_64.exe -cdrom $(patsubst /mnt/c/%, C:/%, $(FINAL_PATH)) -s -S \
		-no-reboot -no-shutdown -monitor stdio \
		-smp 4 -m 512m 

clean:  
	@echo "Cleaning up..."
	@rm -rf Build
	@rm -rf Dist
	@echo "Done."