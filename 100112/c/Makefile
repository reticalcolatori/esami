all:
	make server
	make cstream
	make cdatagram
server:
	gcc -g Select_Server.c -o server
cstream:
	gcc -g Select_Client_Stream.c -o cstream
cdatagram:
	gcc -g Select_Client_Datagram.c -o cdatagram
clean:
	rm server cstream cdatagram
