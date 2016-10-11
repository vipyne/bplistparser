// input binary file path, output json

#include <stdio.h>
#include <errno.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

struct Metadata {
	uint32_t offsetOfOffsetTable;
	uint32_t indexOfRootObject;
	uint32_t item_count;

	uint8_t bytes_per_index; // 1, 2
	uint8_t bytes_per_offset; // 1, 2, 3, or 4
} metadata;

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

struct Metadata get_metadata_values (FILE* file)
{
	struct Metadata file_metadata;

	seek_from_end(file, -4);
	// reading bytes in big endian - reading in the offset of offset table
	errno = 0;
	file_metadata.offsetOfOffsetTable = read_big_endian_32(file);
	if (errno != 0)
	{
		printf("offset of offset table: %d\n", file_metadata.offsetOfOffsetTable);
		printf("error: %d\n", errno);
	}		

	seek_from_end(file, -12);
	// reading bytes in big endian - reading in the index of root object
	errno = 0;
	file_metadata.indexOfRootObject = read_big_endian_32(file);
	if (errno != 0)
	{
		printf("index of root object: %d\n", file_metadata.indexOfRootObject);
		printf("error: %d\n", errno);
	}
			
	seek_from_end(file, -20);
	// reading bytes in big endian - reading in the item count
	errno = 0;
	file_metadata.item_count = read_big_endian_32(file);
	if (errno != 0)
	{
		printf("item count: %d\n", file_metadata.item_count);
		printf("error: %d\n", errno);
	}

	uint8_t length_params[2];
	seek_from_end(file, -26);		
	int res = fread(&length_params, 1, 2, file);
	file_metadata.bytes_per_offset = length_params[0];
	file_metadata.bytes_per_index = length_params[1];
	
	if (res != 2)
		printf("error - length_params: %d, %d\n", length_params[0], length_params[1]);
	
	return file_metadata;
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
	firstEightBytes[8] = 0;	// set null terminator
	fread(&firstEightBytes, 1, 8, filePointer);
	
	int res = strncmp(firstEightBytes, "bplist00", 8);
	
	if (res)
	{
		printf("result: %d\n", res);
		printf("first 8 bytes are: %s\n", firstEightBytes); 
		return 1;
	}	
	
	struct Metadata this_file_metadata = get_metadata_values(filePointer);	

	printf("offset %d\n", this_file_metadata.offsetOfOffsetTable);
	printf("indexOfRootObject %d\n", this_file_metadata.indexOfRootObject);
	printf("item_count %d\n", this_file_metadata.item_count);

	printf("bytes_per_index %d\n", this_file_metadata.bytes_per_index); // 1, 2
	printf("bytes_per_offset %d\n", this_file_metadata.bytes_per_offset); // 1, 2, 3, or 4

	return 0;
}
