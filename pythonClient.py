#UDPClient.py

from socket import *
serverName = ‘measurement_server’
serverPort = 9000
clientSocket = socket(socket.AF_INET, socket.SOCK_DGRAM)
message = raw_input(’Input lowercase sentence:’) #Input som parametre til funk. i stedet?
clientSocket.sendto(message,(serverName, serverPort))

measureMessage, serverAddress = clientSocket.recvfrom(2048)
print measureMessage
clientSocket.close()