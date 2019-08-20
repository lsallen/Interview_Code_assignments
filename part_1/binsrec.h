#define MODE_1 1 // user only giving [input_file_name], the result write to default file a.srec
#define MODE_2 2 // user giving [input_file_name] [output_file_name]
#define DE_FILE_NAME "a.srec"
#define HEADER "S00600004844521B\n"  //default header
#define RECORD 4 // two bytes for type and two bytes for count
#define CHECKSUM 3
#define S5_REC 10 //  one example of S5030004F8
#define S_REC 14 
#define max(a,b) (((a)>(b))?(a):(b))
#define min(a,b) (((a)<(b))?(a):(b))

char * in_file;  // input file pointer
char * out_file; // output file pointer
FILE * infile;
bool SCREEN = false; // whethter to print file contents on screen

uint32_t cur_addr = 0;       // present address
uint32_t end_addr;         // end adress
uint32_t addr_bytes = 2;   // length of address
uint32_t line_length = 32; // max length of bits per line



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


