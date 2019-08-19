# Contributor

## Shan Liang

# Submitted source files

binsrec.c	
binsrec.h
srecbin.c
srecbin.h

# Functionality

## Running environment : Ubunto Linux or Mac OS

There are two separate programs with one Makefile. The first program(binsrec.c binsrec.h) is to read in a binary file and convert it to Motorola S-record file(http://www.amelek.gda.pl/avr/uisp/srecord.htm). The converted S-record file will contain the default header,"S00600004844521B". Default starting address is 0h and converting with a offset is not allowed in my code.
Users have to provide input file name when running the executable file and it is optional to provide the output file name. In my implementation, the default output file name is a.srec.

The second program is to read in a S-record file and convert it to binary file. And my program will detect the input file by looking up the header record "S00600004844521B". If it not exists, input file will not be regarded as a S-record file and will exit with reasonable prompt. Offset address is not allowed and input file need be provided. Default output file name is a.bin.

One more thing needs to mention here is that when converting binary to S-record wihtout specifying output file name, besides writing to default file, all the information will be printed out on screen.


# Compilation Instructions

After downloading the project and loading the current working directory:

 $ make
 
it will compile the source code using GCC compiler and generate two executable files: bin2serc & srec2bin.
 
## Binary to S-Record

### Mode_1 : giving input file name without providing output file. You should create a bin file in that folder.

$ ./bin2srec input_file_name

Under Moder_1, it will write the file to a.srec and print all the necessary info on screen including the file contents.

### Mode_2 : giving input name and output name, the order matters

$ ./bin2srec input_file_name output_file_name

Under Mode_2, it will write the file to a.bin and just print the necessary prompt for processing.

# Error_checking

I did cover the basic error checking for those necessary like fopen, malloc and etc as well as input arguments checking.
Since the time is limited. i did not write the error header file to track the return error type.



