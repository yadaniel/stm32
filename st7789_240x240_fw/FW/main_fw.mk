vpath %.c ./FW
vpath %.h ./FW
VPATH = ./FW

######################################
# FW_CFLAGS
######################################
FW_CFLAGS += -Wall

######################################
# FW_LDFLAGS
######################################
# FW_LDFLAGS += -u _printf_float
FW_LDFLAGS =

######################################
# source
######################################
# C sources
C_SOURCES += \
crc_flash.c \
uart2_printf.c \
st7789.c \
fonts.c

#######################################
# checksum
#######################################
SR=srec_cat

# STM32F303RC => C = 256kB = 0x40000
# STM32L432KC => C = 256kB = 0x40000
# memory mapping from datasheet states that FLASH is mapped into [0x0800_0000 ... 0x0804_0000]
# CRC checksum placed at the end of FLASH => 0x0803_FFFC

HEX_INFILE = ./$(BUILD_DIR)/$(TARGET).hex
HEX_OUTFILE1 = ./$(BUILD_DIR)/$(TARGET)_filled.hex
HEX_OUTFILE2 = ./$(BUILD_DIR)/$(TARGET)_filled_signed.hex
HEX_OUTFILE3 = ./$(BUILD_DIR)/$(TARGET)_signed.hex

# checksum: $(HEX_INFILE)
# 	@# the linker omits blocks of 0xFF to speed up  programming => required for checksum calculation
# 	@$(SR) $(HEX_INFILE) -intel -fill 0xFF 0x08000000 0x08040000 -o $(HEX_OUTFILE1) -intel
# 	@# place checksum at address 0x0803FFFC
# 	@$(SR) $(HEX_OUTFILE1) -intel -crop 0x08000000 0x0803FFFC -STM32 0x0803FFFC -o $(HEX_OUTFILE2) -intel
# 	@# remove 0xFF to speed up programming
# 	@$(SR) $(HEX_OUTFILE2) -intel -unfill 0xFF -output_block_size=16 -address_length=4 -o $(HEX_OUTFILE3) -intel
#
#define ADDR_FLASH_PAGE_125  ((uint32_t)0x0803E800) 	// checksum page, checksum at (0x0803F000-4) == 0x0803EFFC
#define ADDR_FLASH_PAGE_126  ((uint32_t)0x0803F000) 	// eeprom emulation page0
#define ADDR_FLASH_PAGE_127  ((uint32_t)0x0803F800) 	// eeprom emulation page1
#define ADDR_FLASH_PAGE_128  ((uint32_t)0x08040000) 	// reserved

checksum: $(HEX_INFILE)
	@# the linker omits blocks of 0xFF to speed up  programming => required for checksum calculation
	@$(SR) $(HEX_INFILE) -intel -fill 0xFF 0x08000000 0x08040000 -o $(HEX_OUTFILE1) -intel
	@# place checksum at address 0x0803EFFC
	@$(SR) $(HEX_OUTFILE1) -intel -crop 0x08000000 0x0803EFFC -STM32 0x0803EFFC -o $(HEX_OUTFILE2) -intel
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

