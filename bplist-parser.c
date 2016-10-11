// input binary file path, output json

#include <stdio.h>
#include <errno.h>
#include <stdint.h>
#include <string.h>

uint32_t read_big_endian_32 (FILE* file)
{
	uint32_t output = 0;
	unsigned char bytes[4];
	int items_read = fread(bytes, 4, 1, file);
	
	if (items_read == 1)
	{
		output = (uint32_t)((bytes[0] << 24) |
							(bytes[1] << 16) |
							(bytes[2] << 8) |
							(bytes[3])); 	
		return output;
	}
	errno = ENOENT;
	printf("eof error %d, file error %d\n", feof(file), ferror(file));
	return 0;
}

int main (int argc, char *argv[])
{
	if (argc != 2)
	{
		printf("i need one argument: return 1");
		return 1;
	}

	FILE *filePointer;
	// r - read
	// b - binary mode
	filePointer = fopen(argv[1], "rb");
	if (filePointer == NULL)
	{
		printf("file not readable because: %d\n", errno);
		return 1;
	}

	char firstEightBytes[9];
	// set null terminator
	firstEightBytes[8] = 0;
	fread(&firstEightBytes, 1, 8, filePointer);
	
	
	int res = strncmp(firstEightBytes, "bplist00", 8);
	
	if (res)
	{
		printf("result: %d\n", res);
		printf("first 8 bytes are: %s\n", firstEightBytes); 
		return 1;
	}	

	long e_offsetFromEnd = -4; // reading from end of file, so must be negative number
	int seekResult = fseek(filePointer, e_offsetFromEnd, SEEK_END);
	if (seekResult)
	{ 
		printf("seek result: %d\n", seekResult);
		return 1;
	}

	// reading bytes in big endian - reading in the offset of offset table
	errno = 0;
	uint32_t offsetOfOffsetTable = read_big_endian_32(filePointer);
	if (errno == 0)
	{
		printf("offset of offset table: %d\n", offsetOfOffsetTable);
	} else {
		printf("error: %d\n", errno);
	}		

	return 0;
}
