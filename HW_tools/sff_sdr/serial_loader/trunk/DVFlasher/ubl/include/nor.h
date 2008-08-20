/* --------------------------------------------------------------------------
    FILE        : nor.h
    PURPOSE     : NOR driver header file
    PROJECT     : DaVinci User Boot-Loader and Flasher
    AUTHOR      : Daniel Allred
    DATE	    : Jan-22-2007

    HISTORY
 	    v1.00 completion
 	        Daniel Allred - Jan-22-2007
 ----------------------------------------------------------------------------- */

 
#ifndef _NOR_H_
#define _NOR_H_

#include "tistdtypes.h"
#include "dm644x.h"

/************* Constants and Macros **************/
#define BUS_8BIT    0x01
#define BUS_16BIT   0x02
#define BUS_32BIT   0x04

/**************** DEFINES for AMD Basic Command Set **************/
#define AMD_CMD0                    0xAA        // AMD CMD PREFIX 0
#define AMD_CMD1                    0x55        // AMD CMD PREFIX 1
#define AMD_CMD0_ADDR               0x555       // AMD CMD0 Offset 
#define AMD_CMD1_ADDR               0x2AA       // AMD CMD1 Offset 
#define AMD_CMD2_ADDR       	    0x555       // AMD CMD2 Offset 
#define AMD_ID_CMD                  0x90        // AMD ID CMD
#define AMD_MANFID_ADDR             0x00        // Manufacturer ID offset
#define AMD_DEVID_ADDR0             0x01        // First device ID offset
#define AMD_DEVID_ADDR1             0x0E        // Offset for 2nd byte of 3 byte ID 
#define AMD_DEVID_ADDR2             0x0F        // Offset for 3rd byte of 3 byte ID 
#define AMD_ID_MULTI                0x7E        // First-byte ID value for 3-byte ID
#define AMD_RESET                   0xF0        // AMD Device Reset Command
#define AMD_BLK_ERASE_SETUP_CMD     0x80        // Block erase setup
#define AMD_BLK_ERASE_CMD	        0x30        // Block erase confirm
#define AMD_BLK_ERASE_DONE	        0xFFFF      // Block erase check value
#define AMD_PROG_CMD                0xA0        // AMD simple Write command
#define AMD_WRT_BUF_LOAD_CMD        0x25        // AMD write buffer load command
#define AMD_WRT_BUF_CONF_CMD        0x29        // AMD write buffer confirm command

/**************** DEFINES for Intel Basic Command Set **************/
#define INTEL_ID_CMD            0x90        // Intel ID CMD
#define INTEL_MANFID_ADDR       0x00        // Manufacturer ID offset
#define INTEL_DEVID_ADDR        0x01        // Device ID offset
#define INTEL_RESET             0xFF        // Intel Device Reset Command
#define INTEL_ERASE_CMD0        0x20        // Intel Erase command
#define INTEL_ERASE_CMD1        0xD0        // Intel Erase command
#define INTEL_WRITE_CMD         0x40        // Intel simple write command
#define INTEL_WRT_BUF_LOAD_CMD  0xE8        // Intel write buffer load command
#define INTEL_WRT_BUF_CONF_CMD  0xD0        // Intel write buffer confirm command
#define INTEL_LOCK_CMD0         0x60        // Intel lock mode command
#define INTEL_LOCK_BLOCK_CMD    0x01        // Intel lock command
#define INTEL_UNLOCK_BLOCK_CMD  0xD0        // Intel unlock command
#define INTEL_CLEARSTATUS_CMD   0x50        // Intel clear status command


/**************** DEFINES for CFI Commands and Table **************/

// CFI Entry and Exit commands
#define CFI_QRY_CMD             0x98U
#define CFI_EXIT_CMD            0xF0U

// CFI address locations
#define CFI_QRY_CMD_ADDR        0x55U

// CFI Table Offsets in Bytes
#define CFI_Q                   0x10
#define CFI_R                   0x11
#define CFI_Y                   0x12
#define CFI_CMDSET              0x13
#define CFI_CMDSETADDR          0x15
#define CFI_ALTCMDSET           0x17
#define CFI_ALTCMDSETADDR       0x19
#define CFI_MINVCC              0x1B
#define CFI_MAXVCC              0x1C
#define CFI_MINVPP              0x1D
#define CFI_MAXVPP              0x1E
#define CFI_TYPBYTEPGMTIME      0x1F
#define CFI_TYPBUFFERPGMTIME    0x20
#define CFI_TYPBLOCKERASETIME   0x21
#define CFI_TYPCHIPERASETIME    0x22
#define CFI_MAXBYTEPGMTIME      0x23
#define CFI_MAXBUFFERPGMTIME    0x24
#define CFI_MAXBLOCKERASETIME   0x25
#define CFI_MAXCHIPERASETIME    0x26
#define CFI_DEVICESIZE          0x27
#define CFI_INTERFACE           0x28
#define CFI_WRITESIZE           0x2A
#define CFI_NUMBLKREGIONS       0x2C
#define CFI_BLKREGIONS          0x2D
#define CFI_BLKREGIONSIZE       0x04

// Maximum number of block regions supported
#define CFI_MAXREGIONS          0x06

/*********************** Enumerated types *************************/
// Supported Flash Manufacturers
enum FlashManufacturerID {
    UNKNOWN_ID = 0x00,
    AMD = 0x01,
    FUJITSU = 0x04,
    INTEL = 0x89,
    MICRON = 0x2C,
    SAMSUNG = 0xEC,
    SHARP = 0xB0
};
typedef enum FlashManufacturerID MANFID;

// Supported CFI command sets
enum FlashCommandSet {
    UNKNOWN_CMDSET = 0x0000,
    INTEL_EXT_CMDSET = 0x0001,
    AMD_BASIC_CMDSET = 0x0002,
    INTEL_BASIC_CMDSET = 0x0003,
    AMD_EXT_CMDSET = 0x0004,
    MITSU_BASIC_CMDSET = 0x0100,
    MITSU_EXT_CMDSET = 0x0101
    
};
typedef enum FlashCommandSet CMDSET;

/*************************** Structs *********************************/
// Struct to hold discovered flash parameters
typedef struct _NOR_MEDIA_STRUCT_ {
   uint32_t       flashBase;                          // 32-bit address of flash start
   uint8_t        busWidth;                           // 8-bit or 16-bit bus width
   uint8_t        chipOperatingWidth;                 // The operating width of each chip
   uint8_t        maxTotalWidth;                      // Maximum extent of width of all chips combined - determines offset shifts
   uint32_t       flashSize;                          // Size of NOR flash regions in bytes (numberDevices * size of one device)
   uint32_t       bufferSize;                         // Size of write buffer
   CMDSET       commandSet;                         // command set id (see CFI documentation)
   uint8_t        numberDevices;                      // Number of deives used in parallel
   uint8_t        numberRegions;                      // Number of regions of contiguous regions of same block size
   uint32_t       numberBlocks[CFI_MAXREGIONS];    // Number of blocks in a region
   uint32_t       blockSize[CFI_MAXREGIONS];       // Size of the blocks in a region
   MANFID       manfID;                             // Manufacturer's ID
   uint16_t       devID1;                             // Device ID
   uint16_t       devID2;                             // Used for AMD 3-byte ID devices
} NOR_INFO, *PNOR_INFO;

typedef union {
	uint8_t c;
	uint16_t w;
	uint32_t l;
} FLASHData;

typedef union {
	volatile uint8_t *cp;
	volatile uint16_t *wp;
	volatile uint32_t *lp;
} FLASHPtr;


/*************************** Function Prototypes *************************/

// Global NOR commands
uint32_t NOR_Init ();
uint32_t NOR_Copy(void);
uint32_t NOR_WriteBytes(uint32_t writeAddress, uint32_t numBytes, uint32_t readAddress);
uint32_t NOR_GlobalErase();
uint32_t NOR_Erase(uint32_t start_address, uint32_t size);
uint32_t DiscoverBlockInfo(uint32_t address,uint32_t* blockSize, uint32_t* blockAddr);

// Flash Identification  and Discovery
uint32_t QueryCFI( uint32_t baseAddress );

// Generic functions to access flash data and CFI tables
volatile uint8_t *flash_make_addr (uint32_t blkAddr, uint32_t offset);
void flash_make_cmd (uint8_t cmd, void *cmdbuf);
void flash_write_cmd (uint32_t blkAddr, uint32_t offset, uint8_t cmd);
void flash_write_data(uint32_t address, uint32_t data);
void flash_write_databuffer(uint32_t* address, void* data, uint32_t numBytes);
uint32_t flash_verify_databuffer(uint32_t address, void* data, uint32_t numBytes);
uint32_t flash_read_data(uint32_t address, uint32_t offset);
FLASHData flash_read_CFI_bytes (uint32_t blkAddr, uint32_t offset, uint8_t numBytes);
Bool flash_isequal (uint32_t blkAddr, uint32_t offset, uint8_t val);
Bool flash_issetall (uint32_t blkAddr, uint32_t offset, uint8_t mask);
Bool flash_issetsome (uint32_t blkAddr, uint32_t offset, uint8_t mask);

// Generic commands that will point to either AMD or Intel command set
uint32_t (* Flash_Write)(uint32_t, volatile uint32_t);
uint32_t (* Flash_BufferWrite)( uint32_t, volatile uint8_t[], uint32_t);
uint32_t (* Flash_Erase)(uint32_t);
uint32_t (* Flash_ID)(uint32_t);

// Empty commands for when neither command set is used
uint32_t Unsupported_Erase( uint32_t );
uint32_t Unsupported_Write( uint32_t, volatile uint32_t );
uint32_t Unsupported_BufferWrite( uint32_t, volatile uint8_t[], uint32_t );
uint32_t Unsupported_ID( uint32_t );

//Intel pointer-mapped commands
uint32_t Intel_Erase( volatile uint32_t blkAddr);
uint32_t Intel_Write( uint32_t address, volatile uint32_t data );
uint32_t Intel_BufferWrite( uint32_t address, volatile uint8_t data[], uint32_t numBytes );
uint32_t Intel_ID( uint32_t );

//AMD pointer-mapped commands
uint32_t AMD_Erase(uint32_t blkAddr);
uint32_t AMD_Write( uint32_t address, volatile uint32_t data );
uint32_t AMD_BufferWrite(uint32_t address, volatile uint8_t data[], uint32_t numBytes );
uint32_t AMD_ID( uint32_t );

// Misc. Intel commands
uint32_t Intel_Clear_Lock(volatile uint32_t blkAddr);
uint32_t Intel_Set_Lock(volatile uint32_t blkAddr);
uint32_t Intel_Lock_Status_Check();
void Intel_Soft_Reset_Flash();
void Intel_Clear_Status();
void Intel_Wait_For_Status_Complete();

// Misc. AMD commands
void AMD_Soft_Reset_Flash();
void AMD_Prefix_Commands();
void AMD_Write_Buf_Abort_Reset_Flash();

#endif //_NOR_H_
