// input binary file path, output json

#include <stdio.h>
#include <errno.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

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

void seek_from_end (FILE* file, size_t offset)
{
	int seekResult = fseek(file, offset, SEEK_END);
	if (seekResult)
	{ 
		printf("seek result: %d\n", seekResult);
		exit(1);
	}
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

	seek_from_end(filePointer, -4);
	// reading bytes in big endian - reading in the offset of offset table
	errno = 0;
	uint32_t offsetOfOffsetTable = read_big_endian_32(filePointer);
	if (errno == 0)
	{
		printf("offset of offset table: %d\n", offsetOfOffsetTable);
	} else {
		printf("error: %d\n", errno);
	}		

	
	seek_from_end(filePointer, -12);
	// reading bytes in big endian - reading in the index of root object
	errno = 0;
	uint32_t indexOfRootObject = read_big_endian_32(filePointer);
	if (errno == 0)
	{
		printf("index of root object: %d\n", indexOfRootObject);
	} else {
		printf("error: %d\n", errno);
	}

			
	seek_from_end(filePointer, -20);
	// reading bytes in big endian - reading in the item count
	errno = 0;
	uint32_t item_count = read_big_endian_32(filePointer);
	if (errno == 0)
	{
		printf("item count: %d\n", item_count);
	} else {
		printf("error: %d\n", errno);
	}

	uint8_t length_params[2];
	seek_from_end(filePointer, -26);		
	fread(&length_params, 1, 2, filePointer);
	printf("length_params: %d, %d\n", length_params[0], length_params[1]);

	return 0;
}
