# Submitted Source files

session.py

libraries:

import xlrd 

from collections import defaultdict

import sys

# Compilation Instruction

## Running environment : Linux OS or MAC OS with python 3


Please copy the data file you want to process to the current directory and run:

$ python session.py

It will read in the data in default file (TestData.xlsx) and process it as required. It will wrtie the result to "session_rec.txt"
If input file doesn't exist or create output file failed, it will print reasonable info on the screen and exit normally

or 

$ python session.py input_file

it will read in input_file and process data as required, It will wrtie the result to "session_rec.txt".

or 
$ python session.py input_file out_file

Similar with former case, under this condition user can define output file name.

