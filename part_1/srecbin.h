
#define MODE_1 1 // user only giving [input_file_name], the result write to default file a.srec
#define MODE_2 2 // user giving [input_file_name] [output_file_name]
#define DE_FILE_NAME "a.bin"
#define HEADER "S00600004844521B\n"
#define MAX_BUFFER_SIZE 514 // 514 bytes is for general use, typically 70 bytes is enough in our case
char * in_file;
char * out_file;
FILE * infile;

uint32_t cur_addr = 0;       // present address
uint32_t end_addr;         // end adress
uint32_t addr_bytes = 2;   // length of address


uint32_t get_file_size(FILE *fp)
{
	struct stat temp;

	if (!fstat(fileno(fp), &temp))
	{
		return((uint32_t)temp.st_size);
	}
	else
	{
		return 0;
	}
}


uint8_t hex_to_int(unsigned char hex)
{
	uint8_t result;
	if(hex >= '0' && hex <= '9')
	{
		result = hex - '0';
	}
	else if (hex >= 'A' && hex <= 'F')
	{
		result = hex - 'A' + 10;
	}
	else if (hex >= 'a' && hex <= 'f')
	{
		result = hex - 'a' + 10;
	}
	else
	{
		result = -1;
	}
	return result;
}
