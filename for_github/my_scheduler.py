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

def job():
	yesterday = datetime.datetime.now() - datetime.timedelta(days = 1)
	
	filename = r"directory"'filename'+time.strftime('%d_%m_%Y')
	
	filename2=r"directory"'current_shecksum'
	try:
		os.remove(filename)
	except OSError:
		pass		
	
	
   

	ip = ''
	username = ''
	password = ''
	COMMAND = "show XXX"
	
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
		output = remote_conn.recv(65535)
		print (output)

		remote_conn.send(COMMAND + '\n')
		time.sleep(1)

		for i in range(0,1):
			remote_conn.send(' ')
			result = remote_conn.recv(5000)
			fileText = result.strip()
			writeFile = open(filename,'ab')
			writeFile.write (fileText)
			print(result)
		
		subprocess.Popen(['directory', 'filename.py'])
	

	except paramiko.ssh_exception.NoValidConnectionsError:
		with open(r"C:directory"'log','a') as file:
			file.write("\n")
			file.write(time.strftime('%d_%m_%Y_%H:%M:%S')+"  Unable to connect to port xx on ip")
			file.write("\n")
	except OSError:
		with open(r"directory"'log','a') as file:
			file.write("\n")
			file.write(time.strftime('%d_%m_%Y_%H:%M:%S')+"  some other error")
			file.write("\n")
	except EOFError:
		with open(r"directory"'log','a') as file:
			file.write("\n")
			file.write(time.strftime('%d_%m_%Y_%H:%M:%S')+"  Unable to connect -the network is down")
			file.write("\n")
	except FileNotFoundError:
		with open(r"directory"'log','a') as file:
			file.write("\n")
			file.write(time.strftime('%d_%m_%Y_%H:%M:%S')+"  Unable to connect -the network is down")
			file.write("\n")
	except IOError:
		with open(r"directory"'log','a') as file:
			file.write("\n")
			file.write(time.strftime('%d_%m_%Y_%H:%M:%S')+"  Unable to connect -the network is down")
			file.write("\n")
		
#schedule.every().day.at("12:29").do(job)
#schedule.every(10).minutes.do(job)
schedule.every(3).hours.do(job)
#schedule.every().monday.do(job)
#schedule.every().tuesday.at("13:15").do(job)
#schedule.every().wednesday.at("13:15").do(job)
#schedule.every().thursday.at("13:15").do(job)
#schedule.every().friday.at("13:15").do(job)
#schedule.every().saturday.at("13:15").do(job)
#schedule.every().sunday.at("13:15").do(job)


while True:
    schedule.run_pending()
    time.sleep(True)






