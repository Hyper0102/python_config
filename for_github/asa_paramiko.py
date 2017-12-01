import paramiko
import time
import datetime
import os
try:
    os.remove('filename.txt')

except OSError:
    pass

ip = ''
username = ''
password = ''
COMMAND = "show run"
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
filename = 'violat_'+time.strftime('%d.%m.%Y')
for i in range(0,250):
	remote_conn.send(' ')
	result = remote_conn.recv(5000)
	fileText = result.strip()
	writeFile = open(filename,'ab')
	writeFile.write (fileText)
	print(result)

writeFile.close

