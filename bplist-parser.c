// input binary file path, output json

#include <stdio.h>
#include <errno.h>
#include <stdint.h>

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
	
	printf("first 8 bytes are: %s\n", firstEightBytes); 
	
	long e_offsetFromEnd = -4; // reading from end of file, so must be negative number
	int seekResult = fseek(filePointer, e_offsetFromEnd, SEEK_END);

	printf("seek result: %d\n", seekResult);
	
	errno = 0;
	uint32_t b_offsetOfOffsetTable = read_big_endian_32(filePointer);
	if (errno == 0)
	{
		printf("offset of offset table: %d\n", b_offsetOfOffsetTable);
	} else {
		printf("error: %d\n", errno);
	}
		
	printf("eof error %d, file error %d\n", feof(filePointer), ferror(filePointer));

	return 0;
}
