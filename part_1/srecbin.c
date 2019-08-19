#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "srecbin.h"




int srec_bin(FILE * srec, char * bin, uint32_t file_size) // this function transfer bin file to S-record file
{
	char buffer[MAX_BUFFER_SIZE]; // extra byte for checksum
	FILE * out;
	uint32_t type,addr_bytes,total_byte;
	fprintf(stdout,"task is under processing ...\n");
	fseek(srec, cur_addr, SEEK_SET); //initialize fp

	if((out = fopen(bin,"wb")) == NULL)    // create file for writing
	{
		fprintf(stderr, "fail on creating output file\n");
		return 3;
	}
	do
	{
		if(fgets(buffer,MAX_BUFFER_SIZE,srec))
		{
			// do nothing
		}
		// printf("%s\n",buffer);
		if(buffer[0] == 'S')
		{
			type = buffer[1] - '0';
			if(type == 0)
			{
				char header[] = HEADER;
				if(strcmp(buffer,header) != 0)
				{
					fprintf(stderr, "input file is not a S-record file\n");
					return 3;
				}
				continue;
			}
			total_byte = (hex_to_int(buffer[2]) << 4) + hex_to_int(buffer[3]);
			if((type >= 1) && (type <= 3))
			{
				addr_bytes = type + 1;
				uint32_t location = 0;
				for(int k = 4; k <(addr_bytes * 2) + 4;k++)
				{
					uint8_t temp = buffer[k];
					temp = hex_to_int(temp);
					if(temp < 0)
					{
						fprintf(stderr, "invalid address parse\n");
						return 4;
					}
					location = location << 4;
					location += temp;
					//printf("location is %d\n",location);
				}
				uint32_t data_byte = total_byte - addr_bytes - 1;
				//printf("data length is %d\n",data_byte);
				char * data =(char *)malloc(sizeof(char) * data_byte);
				if(data == NULL)
				{
					fprintf(stderr, "invalid malloc\n");
					return 3;
				}
				int q = 0;
				for(int i = 4 + addr_bytes * 2;i < (4 + addr_bytes * 2 + 2 * data_byte);i +=2)
				{
					data[q] = (hex_to_int(buffer[i]) << 4) + hex_to_int(buffer[i + 1]);
					//printf("%d",data[q]);
					q++;
				}
				fseek(out,location,SEEK_SET);
				fwrite(data,sizeof(char),data_byte,out);
			}
		}
		else
		{
			fprintf(stderr, "invalid input file\n");
			return 3;
		}
	}while(!feof(srec));	
	fclose(out);
	printf("process complete\n");
	return 0;
}


int main(int argc, char *argv[])
{
	uint32_t size;     // utilize a 32 bits int to hold the size of file, max size 2^32-1
	switch(argc)
	{
		case 2 :
		in_file = argv[MODE_1];
		char temp[] =  DE_FILE_NAME;
		out_file = temp;
		break;

		case 3:
		in_file = argv[MODE_1];
		out_file = argv[MODE_2];
		break;

		default:
		fprintf(stderr, "invalid input cmd\n");
		return 1; 
	}

	if (in_file == NULL)
	{
		fprintf(stderr, "\n** No input filename specified\n");
		return 1;
	}
	
	if ((infile = fopen(in_file, "rb")) != NULL)  
	{
		size = get_file_size(infile) - 1;
		//printf("file size is %d\n",size);
		end_addr = size;
		if(cur_addr > size)
		{
			fprintf(stderr, "invalid begin address,larger than file size\n");
			return 2;
		}
		if(end_addr < cur_addr)
		{
			fprintf(stderr, "end address less than begin address\n");
			return 2;
		}
		fprintf(stdout, "Convert S-record to binary file, assuming the existence of S-record header \n");
		fprintf(stdout, "input S-record file is : %s\n",in_file);
		fprintf(stdout, "output bin file name is : %s\n",out_file);
		if(strcmp(in_file,out_file) == 0)
		{
			fprintf(stderr, "Invalid: same input and output file name\n");
			return 1;
		}
		if(srec_bin(infile,out_file,size) != 0)
		{
			fprintf(stdout, "Convert failed\n");
		}
		fclose(infile);
		return 0;
	}
	else
	{
		fprintf(stderr, "Input file %s not found\n", in_file);
		return 2;
	}
}
