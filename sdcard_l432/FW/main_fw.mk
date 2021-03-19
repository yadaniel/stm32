vpath %.c ./FW
vpath %.h ./FW
VPATH = ./FW

######################################
# source
######################################
# C sources
C_SOURCES += \
FW/main_fw.c \
FW/user_diskio.c \
FW/ff_gen_drv.c \
FW/diskio.c \
FW/fatfs.c \
FW/ff.c

# FW/sdcard.c

######################################
# FW_DATETIME
######################################
FW_DATETIME = $(shell "./_tools/fw_build_datetime.py")
ifneq ($(FW_DATETIME),0)
$(error "fw build script failed with $(FW_DATETIME))
endif

######################################
# FW_LDFLAGS
######################################
# FW_LDFLAGS += -u _printf_float

######################################
# FW_CFLAGS
######################################
# used temporary to disable warnings
FW_CFLAGS += -Wno-unused-variable -Wno-unused-but-set-variable -Wno-unused-function

#######################################
# checksum
#######################################
SR=srec_cat

#######################################
# all
#######################################
all:
	@echo "usage: make flash_checksum"

# STM32F303RC => C = 256kB = 0x40000
# STM32L432KC => C = 256kB = 0x40000
# memory mapping from datasheet states that FLASH is mapped into [0x0800_0000 ... 0x0804_0000]
# CRC checksum placed at the end of FLASH => 0x0803_FFFC

HEX_INFILE = ./$(BUILD_DIR)/$(TARGET).hex
HEX_OUTFILE1 = ./$(BUILD_DIR)/$(TARGET)_filled.hex
HEX_OUTFILE2 = ./$(BUILD_DIR)/$(TARGET)_filled_signed.hex
HEX_OUTFILE3 = ./$(BUILD_DIR)/$(TARGET)_signed.hex

format:
	@$(foreach f, $(C_SOURCES), astyle --suffix=none --style=java --pad-oper --add-brackets --add-one-line-brackets $(f);)
	@$(foreach f, $(C_SOURCES), astyle --suffix=none --style=java --pad-oper --add-brackets --add-one-line-brackets $(f);)

#######################################
# size
#######################################
size:
	@echo "size of complete FW"
	arm-none-eabi-size.exe ./$(BUILD_DIR)/$(TARGET).elf
	@echo
	@echo "size of largest object files"
	arm-none-eabi-size.exe ./$(BUILD_DIR)/*.o | sort -r | head -10

#######################################
# tags
#######################################
tags:
	@echo "making tags ..."
	ctags -R .

#######################################
# md5
#######################################
md5:
	@echo "making checksums ..."
	@rm -f _verify/md5 && touch _verify/md5
	@rm -f _verify/sha256 && touch _verify/sha256
	@md5sum.exe ./$(BUILD_DIR)/$(TARGET).elf >> _verify/md5
	@$(foreach f, $(C_SOURCES), md5sum $(f) >> _verify/md5;)
	@sha256sum.exe ./$(BUILD_DIR)/$(TARGET).elf >> _verify/sha256
	@for f in $(C_SOURCES); do sha256sum.exe $$f >> _verify/sha256; done

checksum: $(HEX_INFILE)
	@# the linker omits blocks of 0xFF to speed up  programming => required for checksum calculation
	@$(SR) $(HEX_INFILE) -intel -fill 0xFF 0x08000000 0x08040000 -o $(HEX_OUTFILE1) -intel
	
	@# place checksum at address 0x0803FFFC
	@$(SR) $(HEX_OUTFILE1) -intel -crop 0x08000000 0x0803FFFC -STM32 0x0803FFFC -o $(HEX_OUTFILE2) -intel
	
	@# remove 0xFF to speed up programming
	@$(SR) $(HEX_OUTFILE2) -intel -unfill 0xFF -output_block_size=16 -address_length=4 -o $(HEX_OUTFILE3) -intel

#######################################
# flash with checksum
# me => full erase required for correct checksum
#######################################
flash_checksum: checksum
	@echo "flashing signed firmware ..."
	ST-LINK_CLI.exe -c swd -me -p $(HEX_OUTFILE3) -v -rst

#######################################
# flash
#######################################
flash:
	@echo "flashing ..."
	ST-LINK_CLI.exe -c swd -p $(HEX_INFILE) -v -rst





