#UDPServer.py
Let’s now take a look at the server side of the application:
from socket import *
serverPort = 9000
serverSocket = socket(AF_INET, SOCK_DGRAM)
serverSocket.bind((’’, serverPort))
print ”The server is ready to receive”
while 1:
	message, clientAddress = serverSocket.recvfrom(2048)
	if ((message == 'u') or (message == 'U')):
		#Send /proc/uptime
		# 1. open 2. read 3. send
		foUpTi = open('proc/uptime','r')
		#Maaske med split og float - http://thesmithfam.org/blog/2005/11/19/python-uptime-script/# 
		upTi = foUpTi.read() 
		foUpTi.close()
		servMessage = upTi
	elif ((message == 'l') or (message == 'L')):
		#Send /proc/loadavg
		# 1. open 2. read 3. send
		foLdAvg = open('proc/loadavg','r')
		#Maaske med split og float- http://thesmithfam.org/blog/2005/11/19/python-uptime-script/# 
		ldAvg = foLdAvg.read()
		foLdAvg.close()
		servMessage = ldAvg

	serverSocket.sendto(servMessage, clientAddress)
