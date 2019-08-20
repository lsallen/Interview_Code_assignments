import xlrd 
from collections import defaultdict
import sys
import os


# set up a dictionary list to store the time_stamp with playerID activity records 
activity = defaultdict(list)
# set up a dictionary list to store the time_stamp with playerID activity nums
activity_num = defaultdict(list)
# set up a dictionary to store date information
date_rec = {} 
# set up a dictionary list to store the time_stamp with playID in HeadImpact sheet
HeadI = defaultdict(list) 


def time_diff(t1,t2):
	"""this function is to calculate time difference, like 201809182125 - 201809182110. assuming t2 - t1, same date"""
	offset = int(t1/10000)
	t1 = t1 - offset * 10000
	t2 = t2 - offset * 10000
	t1_h = int (t1/100) 
	t2_h = int (t2/100)
	hd = t2_h - t1_h
	md = (t2 - t2_h * 100)  - (t1 - t1_h * 100)
	difference = md + hd * 60
	return difference

def time_trim(t,num):
	""" this function is to add or minus num (abs < 60) of mins to time t, assuming this operation won't influence date"""
	offset = int(t/10000)
	t = t - offset * 10000
	t_h = int (t/100) 
	t_m = int (t - t_h * 100)
	new = t_m + num
	if new < 0:
		t_m = new + 60
		t_h = t_h -1
	if new >= 60:
		t_m = new - 60
		t_h = t_h + 1
	if t_h >= 24:	
		return ((offset+1) * 10000 + (t_h - 24) * 100 + t_m)
	else:
		return (offset * 10000 + t_h * 100 + t_m)

def string_2_int(str1):
	""" this funtion is to transform a string('2018-09-18 21:25:00') into int 201809182125 """
	result = str1.translate({ord('-'): None})
	result = result.translate({ord(':'): None})
	result = result.translate({ord(' '): None})		
	result = int (result.translate({ord("'"): None}))	
	return int(result/100)

def int_2_string(num1):
	""" this funtion is to transform a num1 201809182125 into string 2018-09-18 21:25:00  """
	temp = str(num1)
	result = [temp[0:4], "-", temp[4:6], "-", temp[6:8], " ", temp[8:10], ":", temp[10:12], ":00"]
	result  = "".join(result)
	return str(result)

def int_2_date(num1):
	""" this funtion is to transform a num1 201809182125 into string and extract the date 2018-09-18  """
	temp = str(num1)
	result = [temp[0:4], "-", temp[4:6], "-", temp[6:8]]
	result = "".join(result)
	return str(result)



def activity_process(wb):
	""" this function is to process the activity record of players and set up the dictionary (time_stamp : player_num)"""
	sheet_1 = wb.sheet_by_index(0)
	for i in range(1,sheet_1.nrows):
		temp = sheet_1.cell_value(i,0)
		playerID = int(temp.strip("'"))
		time_stamp = string_2_int(sheet_1.cell_value(i,1))
		activity[time_stamp].append(playerID)
	# sort the activity keys and set up the dictionary (time_stamp : player_num)
	activity_order = sorted(activity.keys())
	old_date = 0
	for i in range(len(activity_order)):
		activity_num[activity_order[i]].append(len(activity[activity_order[i]]))
		date = int(activity_order[i]/10000)
		if date > old_date:
			date_rec[date] = i
			old_date = date
	return activity_order

def HeadI_process(wb):
	sheet_2 = wb.sheet_by_index(1)
	for i in range(1,sheet_2.nrows):
		temp = sheet_2.cell_value(i,0)
		playerID = int(temp.strip("'"))
		HeadI[string_2_int(sheet_2.cell_value(i,1))].append(playerID)
	HeadI_order = sorted(HeadI.keys())
	return HeadI_order

def ID_read(wb):
	""" get all the player ID information, and set int limit to 15% of players """
	ID_record = set()
	sheet_3 = wb.sheet_by_index(2)
	for k in range(1,sheet_3.nrows):
		temp = sheet_3.cell_value(k,0)
		ID = int(temp.strip("'"))
		ID_record.add(ID)
    # set the limit to 15% of the player nums
	limit = int(len(ID_record) * 0.15)
	return limit

def session_process(activity_order,limit,file_rec):
	# find out all the time_stamps meeting the specs by date
	session_bool = False
	for i in range(0,len(date_rec)):
		start = date_rec.get(list(date_rec)[i])
		if i == len(date_rec) - 1:
			end = len(activity_order)
		else:
			end = date_rec.get(list(date_rec)[i+1])
		Day_Session_rec = list()
		for k in range(start,end,1):
			if activity_num.get(activity_order[k])[0] >= limit:  # larger than 15%  of players
				if activity_order[k] in HeadI.keys():
					Day_Session_rec.insert(len(Day_Session_rec),activity_order[k])
				else:
					offset = 1
					while True:
						# if within 5 min range, the total rec num exceed the limit
						if (((k+offset) < (end - 1)) and (time_diff(activity_order[k],activity_order[k+offset]) <= 5)):
							if activity_order[k+offset] in HeadI.keys():
								Day_Session_rec.insert(len(Day_Session_rec),activity_order[k])
								break
							else:
								offset += 1
						else:
							break
			else:
				num = activity_num.get(activity_order[k])[0]
				offset = 1
				while True:
					if (((k+offset) < (end - 1)) and (time_diff(activity_order[k],activity_order[k+offset]) <= 5)):
						num = num + activity_num.get(activity_order[k+offset])[0]
						if num >= limit:
							for q in range(1,offset):
								if activity_order[q+offset] in HeadI.keys():
									session_bool = True
									Day_Session_rec.insert(len(Day_Session_rec),activity_order[k])
									break
							if session_bool:
								session_begin = False
								break
							else:
								offset += 1
						else:

							offset += 1
					else:
						break
		# determine the session begin and end records by date
		if len(Day_Session_rec) > 1:
			today = int_2_date(Day_Session_rec[0])
			file_rec.write("Date: %s \n" %(today))
			file_rec.write("     session begin             session end\n")
			session ={} 
			Day_Session_rec = sorted(Day_Session_rec)
			i = 0
			while True: 
				if i < (len(Day_Session_rec) - 1) :
					if time_diff(Day_Session_rec[i],Day_Session_rec[i+1]) <= 30:
						session[Day_Session_rec[i]] = Day_Session_rec[i+1]
						i = i + 2
						if i < (len(Day_Session_rec) - 1):
							continue
						else:
							break
				if any(session):  #session is not empty
					last_key = list(session.keys())[len(session)-1]
					# to guarntee the same day
					if int(time_trim(session[last_key],40)/10000) == int (session[last_key]/10000):	
						session[last_key] = time_trim(session[last_key],40)
				i += 1
				if i >= (len(Day_Session_rec) - 1):
					break
			# write session record to output file
			for k in range(0,len(session)):
				temp = list(session.keys())[k]
				if (time_trim(temp,-10)/10000) == int (temp/10000):
					temp = time_trim(temp,-10)
				begin = int_2_string(temp)
				temp = list(session.values())[k]
				end = int_2_string(temp)
				file_rec.write("%2d. %s      %s\n" % (k+1,begin,end))


def main():

	if len(sys.argv) == 1:
		# default input file name and output file name
		input_file = "TestData.xlsx"
		output_file = "session_rec.txt"
	if len(sys.argv) == 2:
		# only giving input file name
		input_file = sys.argv[1]
		output_file = "session_rec.txt"
	if len(sys.argv) == 3:
		# user defined input and output file name
		input_file = sys.argv[1]
		output_file = sys.argv[2]

	# read data from input file
	if not os.path.exists(input_file):
		# read failed, prompt and exit			
		print("open file failed, no such file")
		sys.exit(-1)
	else:
		print("open %s successfuelly" % (input_file))
		file_rec = open(output_file,"w+")
		if file_rec == FileNotFoundError:
			print("create output file failed")
			sys.exit()
		else:
			print("create output file successfuelly")
			file_rec.write("/****** session record *********/\n")
	print("data starts being processed")
	wb = xlrd.open_workbook(input_file)
	activity_order = activity_process(wb)
	HeadI_order = HeadI_process(wb)
	limit = ID_read(wb)
	session_process(activity_order,limit,file_rec)
	print("process complete writing result to %s" %(output_file))
	file_rec.close()
	sys.exit(0)
	
	
# run main function
main()
