//	ERROR CODE
#define SUCCESS						0
#define	INVALID_BLOCK				1
#define NOT_AVAIL_REMAP_BLOCK		2
#define REMAP_TABLE_ERASE_FAIL		3
#define REMAP_TABLE_WRITE_FAIL		4
#define REMAP_TABLE_VERIFY_FAIL		5

#define REMAP_TABLE_DISABLE				(0xFFFE)

//	Available Remap Block is 23 Block
#define NAND_REMAP_TABLE_BLOCK			((unsigned int)1000)
#define NAND_REMAP_START_BLOCK			((unsigned int)1001)
#define NAND_REMAP_END_BLOCK			((unsigned int)1023)
#define NAND_MAX_BLOCK	1024

extern U32 Check_Remap_Address(U32 addr, int* err);
extern U32 Check_Remap_Block(U32 block);
extern U32 Trace_Nand_Remap(U32 start, U32 val);

extern U32 Format_Nand_Read_Bytes(U32 addr, int *data, int byte);
extern U32 Format_Nand_Read_Block(U32 block,U8 *buffer);
extern U32 Format_Nand_Verify_Erased_Block(U32 block);
extern U32 Format_Nand_Erase_Block(U32 block);
extern U32 Format_Nand_Verify_Block(U32 block,U8 *buffer);
extern U32 Format_Nand_Write_Block(U32 block, U8 *buffer);
extern int NF8_Read_nByte_SA(int addr, int *data, int byte);
