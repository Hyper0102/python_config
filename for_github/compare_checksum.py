import schedule
import time
import paramiko
import time
import os
import diffios
from pprint import pprint
import difflib
import datetime
import filecmp
import subprocess

yesterday = datetime.datetime.now() - datetime.timedelta(days = 1)

filename = r"directory"'filename'+time.strftime('%d_%m_%Y')


filename_obr = r"directory"'filename'+time.strftime('%d_%m_%Y')+'obr'

filename2=r"directory"'filename'

yesterday_checksum=r"directory"'filename' 
filename2=r"directory"'filename'
z=0

for n in range(1,91):	
	if not os.path.exists(yesterday_checksum):
		z=2
		#n+=1
		date_N_days_ago = datetime.datetime.now() - datetime.timedelta(days = n)
		yesterday_checksum= r"directory"'show_checksum_'+date_N_days_ago.strftime('%d_%m_%Y')
		
	else:
		pass 
		
		
if z==2:
	lines = open(yesterday_checksum, 'r').readlines()
	with open(filename2, 'w') as file_object:
		for line in lines:
			if "ASA-1/act/pri#" not in line:
				file_object.write(line)    
					
lines = open(filename, 'r').readlines()
with open(filename_obr, 'w') as file_object:
	for line in lines:
		if "ASA-1/act/pri#" not in line:
			file_object.write(line)     		
	
a=filecmp.cmp(filename_obr,filename2,shallow=False)
print(a)
print(filename_obr)
print(filename2)	
	
if a==False:
		
	with open(r"directory"'log','a') as file:
		file.write("\n")
		file.write(time.strftime('%d_%m_%Y_%H:%M:%S')+"  Changes were made")
		file.write("\n")	
	lines = open(filename_obr, 'r').readlines()
	with open(filename2, 'w') as file_object:
		for line in lines:
			file_object.write(line)
	subprocess.Popen(['directory', 'filename'])
else:
	with open(r"directory"'log','a') as file:
		file.write("\n")
		file.write(time.strftime('%d_%m_%Y_%H:%M:%S')+"  No changes were made")
		file.write("\n")


