import paramiko
import time
import os
import diffios
from pprint import pprint
import difflib
import datetime
import subprocess

yesterday = datetime.datetime.now() - datetime.timedelta(days = 1)
yesterday_show_run=r"filename"'current_show_run'  

filename6 =  r"directory"'show_run_'+time.strftime('%d_%m_%Y')

asa_show_version = r"directory"'asa_show_version_'+time.strftime('%d_%m_%Y')
asa_show_version2 = r"directory"'asa_show_version_'+time.strftime('%d_%m_%Y')+'_obr'
testfile_no_more = r"directory"'filename.txt'
last_modified = r"directory"'last modified_'+time.strftime('%d_%m_%Y')
in_file_all= r"directory"'changes_in_config_'+time.strftime('%d_%m_%Y')+"_1"

last_modified = r"directory"'last modified_'+time.strftime('%d_%m_%Y')

try:
       os.remove(filename6)
except OSError:
    pass
        
try:   
	os.remove(asa_show_version)
except OSError:
	pass
	
try:   
	os.remove(asa_show_version2)
except OSError:
	pass
				
try:   
	os.remove(last_modified)
except OSError:
	pass	
		
try:   
	os.remove(testfile_no_more)
except OSError:
	pass
	
	
try:   
	os.remove(in_file2_2)
except OSError:
	pass	
	
try:   
	os.remove(in_file1_2)
except OSError:
	pass
	
try:   
	os.remove(in_file2)
except OSError:
	pass		
	
try:   
	os.remove(in_file1)
except OSError:
	pass
					   

ip = ''
username = ''
password = ''
COMMAND = "show run"

try:
	remote_conn_pre=paramiko.SSHClient()
	remote_conn_pre.set_missing_host_key_policy(paramiko.AutoAddPolicy())
	remote_conn_pre.connect(ip, username=username, password=password,
							look_for_keys=False, allow_agent=False)
	remote_conn = remote_conn_pre.invoke_shell()
	remote_conn.send('enable\n')
	time.sleep(1)
	remote_conn.send(password + '\n')
	time.sleep(1)
	
	#socket.recv(bufsize[, flags]) - Receive data from the socket. The return value is a bytes object representing the data received.
	#The maximum amount of data to be received at once is specified by bufsize.
	#See the Unix manual page recv(2) for the meaning of the optional argument flags; it defaults to zero.
	output = remote_conn.recv(4096)
	
	# For best match with hardware and network realities,
	#the value of bufsize should be a relatively small power of 2, for example, 4096
	print (output)

	remote_conn.send(COMMAND + '\n')
	time.sleep(1)

	for i in range(0,250):
		remote_conn.send(' ')
		time.sleep(1)
		result = remote_conn.recv(4096)
		fileText = result.strip()
		writeFile = open(filename6,'ab')
		writeFile.write (fileText)
		print(result)
	
	COMMAND = "show version"
	remote_conn.send(COMMAND + '\n')
	time.sleep(1)

	for i in range(0,15):
		remote_conn.send(' ')
		time.sleep(1)
		result = remote_conn.recv(4096)
		fileText = result.strip()
		writeFile = open(asa_show_version,'ab')
		writeFile.write (fileText)
		print(result)
	
except paramiko.ssh_exception.NoValidConnectionsError:
	#with open - the file is properly closed after its suite finishes, even if an exception is raised on the way.
		with open(r"directory"'log','a') as file:
			file.write("\n")
			file.write(time.strftime('%d_%m_%Y_%H:%M:%S')+"  Unable to connect to port ")
			file.write("\n")

except OSError:
	with open(r"directory"'log','a') as file:
			file.write("\n")
			file.write(time.strftime('%d_%m_%Y_%H:%M:%S')+"  Unable to connect to port  - some other problems")
			file.write("\n")

except EOFError:	
	with open(r"direcroty"'log','a') as file:
			file.write("\n")
			file.write(time.strftime('%d_%m_%Y_%H:%M:%S')+"  Unable to connect to port- some other problems")
			file.write("\n")	
			
except FileNotFoundError:

	with open(r"directory"'log','a') as file:
			file.write("\n")
			file.write(time.strftime('%d_%m_%Y_%H:%M:%S')+"  Unable to connect to port  - some other problems")
			file.write("\n")	
						
except IOError:

	with open(r"directory"'log','a') as file:
			file.write("\n")
			file.write(time.strftime('%d_%m_%Y_%H:%M:%S')+"  Unable to connect to port some other problems")
			file.write("\n")	
			

try:
	lines = open(filename6, 'r').readlines()
	with open(filename7, 'w') as file_object:
		for line in lines:		
			line = line.replace("<--- More --->","")
			line = line.replace(": end", "")
			line = line.replace("show run","")
			line = line.replace("sh run","")
			line = line.replace("  ","")  
			line = line.replace("$","")
			line = line.replace("!","")	
			line = line.replace(": Saved","")
			line = line.replace(" \n","\n")
			line = line.replace(" \r","\r")
			line = line.replace(" \r\n","\r\n")
			line = line.replace(" \t","\t")
			line = line.replace(" \v","\v")
			line = line.replace(" \b","\b")
			line = line.replace(" \f","\f")
			line = line.replace(" \a","\a")
			line=line.lstrip()	
			file_object.write(line)
		
	lines = open(filename7, 'r').readlines()
	with open(filename7,'w') as file:
		for line in lines:
			if not line.isspace() and "=~" not in line and "" not in line and "Cryptochecksum" not in line:
				file.write(line)
 
except FileNotFoundError:s
	with open(r"filename"'log','a') as file:
			file.write("\n")
			file.write(time.strftime('%d_%m_%Y_%H:%M:%S')+"  Could not open file 'show run today' was not found")
			file.write("\n")           

try:
	yesterday_show_run=r"filename"'current_show_run'  
	filename8=r"filename"'current_show_run'  
	k=0

	for n in range(1,91):	
		if not os.path.exists(yesterday_show_run):
			k=2
			#n+=1
			date_N_days_ago = datetime.datetime.now() - datetime.timedelta(days = n)
			yesterday_show_run= r"filename"'show_run_'+date_N_days_ago.strftime('%d_%m_%Y')
		
		else:
			pass 
		
	if k==2:
		lines = open(yesterday_show_run, 'r').readlines()
		with open(filename8, 'w') as file_object:
			for line in lines:
				line = line.replace("<--- More --->","")
				line = line.replace("ASA-1/act/pri#","")
				line = line.replace(": end", "")
				line = line.replace(": Saved","")
				line = line.replace(" \n","\n")
				line = line.replace(" \r","\r")
				line = line.replace(" \t","\t")
				line = line.replace(" \v","\v")
				line = line.replace(" \b","\b")
				line = line.replace(" \f","\f")
				line = line.replace(" \a","\a")
				line = line.replace(" \r\n","\r\n")
				line = line.replace("  ","")             
				line = line.replace("$","")
				line = line.replace("!","")
				line=line.lstrip()			
				file_object.write(line)
		
		lines = open(filename8, 'r').readlines()
		with open(filename8,'w') as file:
			for line in lines:
				if not line.isspace() and "=~" not in line and "" not in line and "Cryptochecksum" not in line:
					file.write(line)
			        
		       				           										

	filename=  asa_show_version
	filename2= asa_show_version2
	filename3= last_modified

	lines = open(filename, 'r').readlines()
	with open(filename2, 'w') as file_object:
		for line in lines:		
			line = line.replace("<--- More --->","")		
			file_object.write(line)
				
	lines = open(filename2, 'r').readlines()
	with open(filename3, 'w') as file_object:
		for line in lines:
			if "Configuration last modified" in line: 
				line = line.replace("Configuration last modified ","")		
				file_object.write(line)

except FileNotFoundError:
	with open(r" filename"'log','a') as file:
			file.write("\n")
			file.write(time.strftime('%d_%m_%Y_%H:%M:%S')+"  Could not open file 'asa show version today' was not found")
			file.write("\n")		

except OSError:
	with open(r"filename"'log','a') as file:
			file.write("\n")
			file.write(time.strftime('%d_%m_%Y_%H:%M:%S')+"  Could not open file 'asa show version today' was not found")
			file.write("\n")

except EOFError:	
	with open(r"filename"'log','a') as file:
			file.write("\n")
			file.write(time.strftime('%d_%m_%Y_%H:%M:%S')+"  Could not open file 'asa show version today' was not found")
			file.write("\n")
			
except IOError:
	with open(r"filename"'log','a') as file:
			file.write("\n")
			file.write(time.strftime('%d_%m_%Y_%H:%M:%S')+"  Could not open file 'asa show version today' was not found")
			file.write("\n")	
						

try:
	with open(file1) as f:
		t1 = f.read().splitlines()
		t1s = set(t1)

	with open(file2) as f:
		t2 = f.read().splitlines()
		t2s = set(t2)


	only_in_file1={}

	for diff in t1s-t2s:
		a=t1.index(diff)
		only_in_file1[a]=diff
    
	only_in_file2={}
	for diff in t2s-t1s:
		b=t2.index(diff)
		only_in_file2[b]=diff

	with open(in_file1, 'w') as f:
		for k in sorted(only_in_file1.keys()):
			value=only_in_file1[k]
			print (k, ':', only_in_file1[k])
			f.write('%s:%s\n' % (k, value))
		
		
	with open(in_file1_2, 'w') as f_out, open(in_file1) as f_in:
		for line in f_in:
			new_str = ','.join(line.split(':')[1:])
			f_out.write(new_str)		


	with open(in_file2, 'w') as f:
		for k in sorted(only_in_file2.keys()):
			value=only_in_file2[k]
			print (k, ':', only_in_file2[k])
			f.write('%s:%s\n' % (k, value))
				
	with open(in_file2_2, 'w') as f_out, open(in_file2) as f_in:
		for line in f_in:
			new_str = ','.join(line.split(':')[1:])
			f_out.write(new_str)
			

except FileNotFoundError:
	pass
except OSError:
	pass
except EOFError:	
	pass
except IOError:
	pass

z=0
for n in range(1,91):	
	if os.path.exists(in_file_all):
		z=2	
		in_file_all= r"directory"'changes_in_config_'+time.strftime('%d_%m_%Y')+'_'+ str(n)		
	else:
		pass 
		

try:
	with open (filename3) as fileobject:
		contents= fileobject.read()	
		
	with open(in_file_all,'a') as file:
		file.write(contents)

	with open(in_file_all, 'a') as f:
		f.write( "\n")
		f.write( "Добавлены в конфигурацию: ")
		f.write( "\n")
		f.write( "\n")
			
	lines = open(in_file1_2, 'r').readlines()
	with open(in_file_all, 'a') as file_object:
		for line in lines:
			if 'access-list' in line:
				file_object.write(line)

	with open(in_file_all, 'a') as f:
		f.write( "\n")

	lines = open(in_file1_2, 'r').readlines()
	with open(in_file_all, 'a') as file_object:
		for line in lines:
			if 'access-list' not in line:
				file_object.write(line)
			
	with open(in_file_all, 'a') as f:
		f.write( "\n")
		f.write( "Удалены из конфигурации: ")
		f.write( "\n")
		f.write( "\n")
	
	lines = open(in_file2_2, 'r').readlines()
	with open(in_file_all, 'a') as file_object:
		for line in lines:
			if 'access-list' in line:
				file_object.write(line)

	with open(in_file_all, 'a') as f:
		f.write( "\n")

	lines = open(in_file2_2, 'r').readlines()
	with open(in_file_all, 'a') as file_object:
		for line in lines:
			if 'access-list' not in line:
				file_object.write(line)

	with open(in_file_all, 'a') as f:
		f.write( "\n")
		f.write( "This message was generated automatically by the python script")


	lines = open(testfile_no_more, 'r').readlines()
	with open(filename8, 'w') as file_object:
		for line in lines:
			file_object.write(line)


	subprocess.Popen(['directory', 'filename.py'])


except FileNotFoundError:
	pass
	
except OSError:
	pass
except EOFError:	
	pass
except IOError:
	pass	
