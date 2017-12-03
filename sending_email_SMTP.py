import getpass
password=''

import smtplib
from email.mime.multipart import MIMEMultipart
from email.mime.text import MIMEText
import time
import paramiko
import time
import os
import diffios
from pprint import pprint
import difflib
import datetime
import subprocess


last_modified = r"directory"'last modified_'+time.strftime('%d_%m_%Y')
n=90
in_file_all= r"directory"'changes_in_config_'+time.strftime('%d_%m_%Y')+'_'+ str(n)	
z=0

while (not os.path.exists(in_file_all)) and (n > 0):		
		
	n-=1		
	in_file_all= r"directory"'changes_in_config_'+time.strftime('%d_%m_%Y')+'_'+ str(n)		
		
####
with open (in_file_all) as fileobject:
	contents1= fileobject.read()
	
fromaddr = 'sender_addr@___com'
toaddr = ['reciever_addr@_com']

msg = MIMEMultipart()
msg['From'] = fromaddr
#msg['To'] = ', '.join(toaddr[0:1])
msg['cc'] = ', '.join([ toaddr[1] ]) 

with open(last_modified) as fileobject:
	contents = fileobject.read()

b= str(n)+' '
msg['Subject'] = 'ASA '+ u' изменения ' + str(b)+ str(contents)

msg.attach(MIMEText(str(contents1)))

p=0

try: 
	server = smtplib.SMTP('ip',port)
	server.starttls()
	server.login(fromaddr, password)
	text = msg.as_string()
	server.sendmail(fromaddr, toaddr, text)
	server.quit()
	print ("Your message is received")
	with open(r"directory"'log','a') as file:
		#file.write("\n")
		file.write(time.strftime('%d_%m_%Y_%H:%M:%S')+" Message was sent to" + str(toaddr))
		file.write("\n")
		
except OSError:
	with open(r"directory"'log','a') as file:
		file.write("\n")
		file.write(time.strftime('%d_%m_%Y_%H:%M:%S')+"  Unable to send the message -the network is down")
		file.write("\n")
		p=9

										
except EOFError:
	with open(r"directory"'log','a') as file:
		file.write("\n")
		file.write(time.strftime('%d_%m_%Y_%H:%M:%S')+"  Unable to send the message -the network is down")
		file.write("\n")
		p=9

except FileNotFoundError:
	with open(r"directory"'log','a') as file:
		file.write("\n")
		file.write(time.strftime('%d_%m_%Y_%H:%M:%S')+"  Unable to send the message -the network is down")
		file.write("\n")
		p=9
		

while p > 0:
	try: 
		time.sleep(60)
		server = smtplib.SMTP('ip',port)
		server.starttls()
		server.login(fromaddr, password)
		text = msg.as_string()
		server.sendmail(fromaddr, toaddr, text)
		server.quit()
		print ("Your message is received")
		with open(r"directory"'log','a') as file:
		#file.write("\n")
			file.write(time.strftime('%d_%m_%Y_%H:%M:%S')+" Message was sent to" + str(toaddr))
			file.write("\n")
		p=0
		
	except OSError:
		with open(r"directory"'log','a') as file:
				file.write("\n")
				file.write(time.strftime('%d_%m_%Y_%H:%M:%S')+"  Unable to send the message -the network is down")
				file.write("\n")
				p-=1

										
	except EOFError:
			with open(r"directory"'log','a') as file:
				file.write("\n")
				file.write(time.strftime('%d_%m_%Y_%H:%M:%S')+"  Unable to send the message -the network is down")
				file.write("\n")
				p-=1
	except FileNotFoundError:
		with open(r"directory"'log','a') as file:
			file.write("\n")
			file.write(time.strftime('%d_%m_%Y_%H:%M:%S')+"  Unable to send the message -the network is down")
			file.write("\n")
			p=9
