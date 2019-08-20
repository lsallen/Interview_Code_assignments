#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "binsrec.h"


int bin_srec(FILE * bin, char * srec, uint32_t file_size) // this function transfer bin file to S-record file
{
	uint8_t checksum;
	uint32_t cur_line,max_addr,total_byte,temp;
	uint32_t rec_num = 0;
	unsigned char buffer[line_length]; 
	FILE * out;

	max_addr = file_size; 
	if ((max_addr > 0xffffl) && (addr_bytes < 3))  // get the num of bytes of address
	{
		addr_bytes = 3;
	}
	if ((max_addr > 0xffffffl) && (addr_bytes < 4))
	{
		addr_bytes = 4;
	}
	fprintf(stdout,"task is under processing ...\n");
	fseek(bin, cur_addr, SEEK_SET); //initialize fp
	if((out = fopen(srec,"w")) == NULL)    // create file for writing
	{
		fprintf(stderr, "fail on create output file\n");
		return 3;
	}
	/* this is to write a header record */
	char header[] = HEADER;
	fwrite(header,1,strlen(header),out);
	if(SCREEN) 
	{
		fprintf(stdout,"%s",header);
	}

	while(1)
	{
		cur_line = min(line_length,(end_addr - cur_addr)+1); // get line number needed to write for this iteration
		total_byte = (addr_bytes + cur_line + 1);
		checksum = total_byte;
		char rec[RECORD+1];
		sprintf(rec,"S%d%02X", addr_bytes - 1, total_byte);
		fwrite(rec,1,strlen(rec),out);
		if(SCREEN) 
		{
			fprintf(stdout,"%s",rec);
		}
		char addr[addr_bytes];
		int j = 0;
		for (int i = addr_bytes - 1; i >= 0; i--) // convert address to hex
		{
			temp = (cur_addr >> (8 * i)) & 0xff;
			sprintf(&addr[j],"%02X",temp);
			checksum += temp;
			j +=2;
		}
		fwrite(addr,sizeof(char),strlen(addr),out);
		if(SCREEN) 
		{
			fprintf(stdout,"%s",addr);
		}
		if(fread(buffer,sizeof(char),cur_line,bin) <= 1) // error or EOF, in this case, we always assume EOF
		{
			// doing nothing 
		}
		char data[2 * cur_line];
		for(int i = 0; i < cur_line; i++)
		{
			sprintf(&data[2 * i],"%02X",buffer[i]);
            checksum += buffer[i];
		}
		fwrite(data,sizeof(char),strlen(data),out);
		if(SCREEN) 
		{
			fprintf(stdout,"%s",data);
		}
		char sum[CHECKSUM+1];
		sprintf(sum,"%02X\n",(255 - checksum));
		fwrite(sum,sizeof(char),strlen(sum),out);
		if(SCREEN) 
		{
			fprintf(stdout,"%s",sum);
		}
		cur_addr += cur_line; // update current address
		rec_num++;
		if (cur_addr - 1 >= max_addr)
		{
			break;
		}
	}
	// S5 record generate
	char S5_head[] = "S5";
	fwrite(S5_head,sizeof(char),strlen(S5_head),out);
	if(SCREEN) 
	{
		fprintf(stdout,"%s",S5_head);
	}
	total_byte = addr_bytes + 1;
	checksum = total_byte; // reset checksum
	char S5_rec[S5_REC];
	int j = 0;
	for (int i = addr_bytes - 1; i >= 0; i--)
	{
		temp = (rec_num >> (8 * i)) & 0xff;
		sprintf(&S5_rec[j],"%02X",temp);
		checksum += temp;
		j +=2;
	}
	sprintf(S5_rec,"%02X%s%02X\n",total_byte,S5_rec,(255 - checksum));
	fwrite(S5_rec,sizeof(char),strlen(S5_rec),out);
	if(SCREEN) 
	{
		fprintf(stdout,"%s",S5_rec);
	}
	// S7 or S8 or S9 generate
	int type = 11 - addr_bytes;
	total_byte = addr_bytes + 1;
	checksum = total_byte;
	char S_head[S_REC];
	sprintf(S_head,"S%d%02X0000%02X\n",type,total_byte,(255 - checksum)); // default starting address is 0
	fwrite(S_head,sizeof(char),strlen(S_head),out);
	if(SCREEN) 
	{
		fprintf(stdout,"%s",S_head);
	}
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
		SCREEN = true;
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
		fprintf(stdout, "Convert binary to S-record file\n");
		fprintf(stdout, "input binary file is : %s\n",in_file);
		fprintf(stdout, "output binary file is : %s\n",out_file);
		if(strcmp(in_file,out_file) == 0)
		{
			fprintf(stderr, "Invalid: same input and output file name\n");
			return 1;
		}
		if(bin_srec(infile,out_file,size) != 0)
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
