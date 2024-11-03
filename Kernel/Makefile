FILENAME = quark
PLATFORM = none
BUILD_VERSION := $(shell ../Scripts/get-version.sh)
BUILD_MODE = release
BUILD_NAME = $(FILENAME)-$(PLATFORM)-v$(BUILD_VERSION)-$(BUILD_MODE)
FINAL_PATH ?= /mnt/d/Codes/Cxxlang/$(BUILD_NAME).iso
BUILD_PATH = Build/platforms/$(PLATFORM)
DIST_PATH = Dist/$(BUILD_NAME).elf
DIST_IMAGE = Dist/$(BUILD_NAME).iso

GRUB_PATH=Target/grub_root
GRUB_CONFIG=$(GRUB_PATH)/boot/grub/grub.cfg  

LIMINE_PATH=Target/limine_root
LIMINE_CONFIG=$(LIMINE_PATH)/limine.cfg

STIVALE_PATH=Target/stivale2_root
STIVALE_CONFIG=$(STIVALE_PATH)/boot/limine.cfg

CXXFLAGS = \
	-Wall -Wextra -Wno-unused-parameter -Wno-sign-compare -Wno-write-strings \
	-Waddress-of-packed-member \
	-DGLOBAL_API_NAMESPACE \
	-ffreestanding -fno-rtti -fno-exceptions -fno-stack-protector -fno-pic \
	-mno-sse -mno-sse2 -mno-sse3 -mno-sse4 -mno-sse4.1 -mno-sse4.2 \
	-mno-avx -mno-avx2 -mno-mmx \
	-std=c++23 \
	-mcmodel=kernel \
	-mno-red-zone \
	-O0 \
	-g  
LDFLAGS = \
 	-static -m elf_x86_64 -nostdlib -z max-page-size=0x1000  

# Source files
DIRS := $(shell find ./Source/$(FILENAME)/ -maxdepth 1 -type d)
DIRS += $(shell find ./Source/libs/ -maxdepth 1 -type d)  
DIRS := $(basename $(patsubst ./%,%,$(DIRS)))  

DRIVERS_FOLDERS := $(shell find ./Source/platforms/impls -type d)
DRIVERS_FOLDERS := $(basename $(patsubst ./%,%,$(DRIVERS_FOLDERS)))

SOURCE_GENERICS := $(foreach dir, $(DIRS), $(wildcard $(dir)/*.cpp))
GENERIC_SOURCE := $(patsubst Source/%.cpp, Build/%.o, $(SOURCE_GENERICS))
DRIVERS_SOURCE := $(patsubst Source/%.cpp, Build/%.o, $(foreach dir, $(DRIVERS_FOLDERS), $(wildcard $(dir)/*.cpp)))

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

check_version:
	@echo "Checking kernel version..."
	@../Scripts/get-version.sh

check_grubscript:
	@echo "Checking grub script..."
	@mkdir -p $(GRUB_PATH)/boot/grub
	@echo 'set timeout=3' 							> $(GRUB_CONFIG)
	@echo 'set default=0' 							>> $(GRUB_CONFIG)
	@echo '' 										>> $(GRUB_CONFIG)
	@echo 'menuentry "$(BUILD_NAME)" {' 			>> $(GRUB_CONFIG)
	@echo '    multiboot2 /boot/$(BUILD_NAME).elf' 	>> $(GRUB_CONFIG)
	@echo '    boot' 								>> $(GRUB_CONFIG)
	@echo '}' 										>> $(GRUB_CONFIG)
	@echo 'menuentry "Reboot" --class=reboot {'  	>> $(GRUB_CONFIG)
	@echo '    reboot' 								>> $(GRUB_CONFIG)
	@echo '}' 										>> $(GRUB_CONFIG)

check_stivalescript:
	@echo "Checking stivale script..."
	@mkdir -p $(STIVALE_PATH)/boot
	@echo 'TIMEOUT=0' 								> $(STIVALE_CONFIG)
	@echo 'VERBOSE=yes' 							>> $(STIVALE_CONFIG)
	@echo '' 										>> $(STIVALE_CONFIG)
	@echo ':$(BUILD_NAME)' 							>> $(STIVALE_CONFIG)
	@echo 'PROTOCOL=stivale2' 						>> $(STIVALE_CONFIG)
	@echo 'KERNEL_PATH=boot:///$(BUILD_NAME).elf' 	>> $(STIVALE_CONFIG)
	@echo 'KASLR=no' 								>> $(STIVALE_CONFIG)
	@echo 'DEPRECATION_WARNING=no' 					>> $(STIVALE_CONFIG)

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
	@echo "AS  $(patsubst Source/%.asm, %.asm, $<)..."
	@$(AS) -f elf64 $< -o $@

auto: check_arguments
	@$(MAKE) clean
	@$(MAKE) build-$(PLATFORM) -j4 PLATFORM=$(PLATFORM) AS=$(AS) CXX=$(CXX) LD=$(LD)  
	@$(MAKE) run-$(PLATFORM) 

build-x86_64: check_stivalescript $(GENERIC_SOURCE) $(DRIVERS_SOURCE) $(PLATFORM_SOURCE) $(PLATFORM_ASSEMBLY)
	@echo "Check prerequisites..."
	@echo "Building x86_64 kernel..."  
	@echo "Making building directory $(BUILD_PATH)."  
	@mkdir -p $(BUILD_PATH)  
	@echo "Making dist directory."  
	@mkdir -p Dist  
	@echo "Linking objects..."  
	@$(LD) $(LDFLAGS) -o $(DIST_PATH) \
		-T Target/LinkerScript-$(PLATFORM).ld \
		$(GENERIC_SOURCE) $(PLATFORM_SOURCE) $(PLATFORM_ASSEMBLY) $(DRIVERS_SOURCE)  
# cp $(DIST_PATH) Target/limine_root/boot/$(FILENAME)-$(PLATFORM)-$(BUILD_MODE).elf  
# xorriso -as mkisofs -b boot/limine/limine-bios-cd.bin \
# 	-no-emul-boot -boot-load-size 4 -boot-info-table \
# 	--efi-boot boot/limine/limine-uefi-cd.bin \
# 	-efi-boot-part --efi-boot-image --protective-msdos-label \
# 	Target/limine_root -o $(DIST_IMAGE)
# ../Thirdparty/limine/limine bios-install $(DIST_IMAGE)  
#
# @cp $(DIST_PATH) $(GRUB_PATH)/boot/$(BUILD_NAME).elf
# grub-mkrescue /usr/lib/grub/i386-pc -o $(DIST_IMAGE) $(GRUB_PATH)  
	@cp $(DIST_PATH) $(STIVALE_PATH)/$(BUILD_NAME).elf
	xorriso -as mkisofs -b boot/limine-cd.bin \
		-no-emul-boot -boot-load-size 4 -boot-info-table \
		--efi-boot boot/limine-cd-efi.bin -efi-boot-part \
		--efi-boot-image --protective-msdos-label \
		$(STIVALE_PATH) -o $(DIST_IMAGE)  
	@cp $(DIST_IMAGE) $(FINAL_PATH)  

run-x86_64:
	@echo "Running x86_64 kernel..."
	@qemu-system-x86_64.exe -cdrom $(patsubst /mnt/d/%, D:/%, $(FINAL_PATH)) -s -S \
		-monitor stdio \
		-cpu qemu64,+ssse3,+sse4.1,+sse4.2 \
		-smp 4 -m 64m 

clean:  
	@echo "Cleaning up..."
	@rm -rf Build
	@rm -rf Dist
	@echo "Done."