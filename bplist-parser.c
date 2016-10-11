// input binary file path, output json

#include <stdio.h>
#include <errno.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

struct Metadata {
	uint32_t offset_of_offset_table;
	uint32_t index_of_root_object;
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

void seek_from_begin (FILE* file, size_t offset)
{
	int seek_result = fseek(file, offset, SEEK_SET);
	if (seek_result)
	{ 
		printf("seek result: %d\n", seek_result);
		exit(1);
	}
}

void seek_from_end (FILE* file, size_t offset)
{
	int seek_result = fseek(file, offset, SEEK_END);
	if (seek_result)
	{ 
		printf("seek result: %d\n", seek_result);
		exit(1);
	}
}

struct Metadata get_metadata_values (FILE* file)
{
	struct Metadata file_metadata;

	seek_from_end(file, -4);
	// reading bytes in big endian - reading in the offset of offset table
	errno = 0;
	file_metadata.offset_of_offset_table = read_big_endian_32(file);
	if (errno != 0)
	{
		printf("offset of offset table: %d\n", file_metadata.offset_of_offset_table);
		printf("error: %d\n", errno);
	}		

	seek_from_end(file, -12);
	// reading bytes in big endian - reading in the index of root object
	errno = 0;
	file_metadata.index_of_root_object = read_big_endian_32(file);
	if (errno != 0)
	{
		printf("index of root object: %d\n", file_metadata.index_of_root_object);
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

	FILE *file_pointer;
	// b - binary mode, r - read
	file_pointer = fopen(argv[1], "rb");
	if (file_pointer == NULL)
	{
		printf("file not readable because: %d\n", errno);
		return 1;
	}

	char first_eight_bytes[9];
	first_eight_bytes[8] = 0;	// set null terminator
	fread(&first_eight_bytes, 1, 8, file_pointer);
	
	int res = strncmp(first_eight_bytes, "bplist00", 8);
	
	if (res)
	{
		printf("result: %d\n", res);
		printf("first 8 bytes are: %s\n", first_eight_bytes); 
		return 1;
	}	
	
	struct Metadata this_file_metadata = get_metadata_values(file_pointer);	

	// print out struct for dev purposes
	printf("offset %d\n", this_file_metadata.offset_of_offset_table);
	printf("index_of_root_object %d\n", this_file_metadata.index_of_root_object);
	printf("item_count %d\n", this_file_metadata.item_count);

	printf("bytes_per_index %d\n", this_file_metadata.bytes_per_index); // 1, 2
	printf("bytes_per_offset %d\n", this_file_metadata.bytes_per_offset); // 1, 2, 3, or 4

	
	seek_from_begin(file_pointer, this_file_metadata.offset_of_offset_table + (this_file_metadata.bytes_per_offset * this_file_metadata.index_of_root_object));

	uint8_t offset_of_root_object = 0;
	
	fread(&offset_of_root_object, 1, 1, file_pointer);
	
	printf("offset_of_root_object : %d\n", offset_of_root_object);
	
	return 0;
}
