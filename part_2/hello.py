# Reading an excel file using Python 
import xlrd 
from collections import defaultdict
def string_2_int(num1):
	""" this funtion is to transform a string('2018-09-18 21:25:00') into int """
	result = num1.translate({ord('-'): None})
	result = result.translate({ord(':'): None})
	result = result.translate({ord(' '): None})		
	result = int (result.translate({ord("'"): None}))	
	return int(result/100)






# Give the location of the file 
data = ("TestData.xlsx") 

file_rec = open("session_rec.txt","a+")
# set up a dictionary list to store the time_stamp with playerID in activity sheet
activity = defaultdict(list)
date_rec = {}   
wb = xlrd.open_workbook(data) 
sheet_1 = wb.sheet_by_index(0)


for i in range(1,sheet_1.nrows):
	temp = sheet_1.cell_value(i,0)
	playerID = int(temp.strip("'"))
	time_stamp = string_2_int(sheet_1.cell_value(i,1))
	activity[time_stamp].append(playerID)

activity_order = sorted(activity.keys())
old_date = 0
activity_num = defaultdict(list)
for i in range(len(activity_order)):
	activity_num[activity_order[i]].append(len(activity[activity_order[i]]))
	date = int(activity_order[i]/10000)
	if date > old_date:
		date_rec[date] = i
		old_date = date
#print(activity_num)


# set up a dictionary list to store the time_stamp with playID in HeadImpact sheet
HeadI = defaultdict(list) 
sheet_2 = wb.sheet_by_index(1)
for i in range(1,sheet_2.nrows):
	temp = sheet_2.cell_value(i,0)
	playerID = int(temp.strip("'"))
	HeadI[string_2_int(sheet_2.cell_value(i,1))].append(playerID)

HeadI_order = sorted(HeadI.keys())
# print(HeadI_order)



# get all the player ID information, and set int limit to 15% of players
ID_record = set()
sheet_3 = wb.sheet_by_index(2)
for k in range(1,sheet_3.nrows):
	temp = sheet_3.cell_value(k,0)
	ID = int(temp.strip("'"))
	ID_record.add(ID)

limit = int(len(ID_record) * 0.15)

session_begin = 1
session_end = 2
session_begin_end = 3
for i in range(0,len(date_rec)):
	start = date_rec.get(list(date_rec)[i])
	if i == len(date_rec) - 1:
		end = len(activity_order)
	else:
		end = date_rec.get(list(date_rec)[i+1])
	Day_Session_rec = {}
	for k in range(start,end,1):
		if activity_num.get(activity_order[k])[0] >= limit:  # larger than 15%  of players
			if activity_order[k] in HeadI.keys():
				print("session begin conditions matches\n")
				print(activity_order[k])
				Day_Session_rec[activity_order[k]] = session_begin
			else:
				offset = 1
				while True:
					if (((k+offset) < (end - 1)) and (activity_order[k+offset] - activity_order[k]) <= 5):
						if activity_order[k+offset] in HeadI.keys():
							print("session begin conditions still matches\n")
							print(activity_order[k])
							Day_Session_rec[activity_order[k]] = session_begin
							break
						else:
							offset += 1
					else:
						break
		else:
			num = activity_num.get(activity_order[k])[0]
			offset = 1
			while True:
				if (((k+offset) < (end - 1)) and (activity_order[k+offset] - activity_order[k]) <= 5):
					num = num + activity_num.get(activity_order[k+offset])[0]
					if num >= limit:
						for q in range(1,offset):
							if activity_order[q+offset] in HeadI.keys():
								session_begin = True
								print("session begin conditions still matches\n")
								print(activity_order[k])
								Day_Session_rec[activity_order[k]] = session_begin
								break
						if session:
							session_begin = False
							break
						else:
							offset +=1
					else:
						offset += 1
				else:
					break
	
















