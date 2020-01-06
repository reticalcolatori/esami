all:
	rpcgen RPC_xFile.x
	gcc RPC_Server.c RPC_xFile_svc.c RPC_xFile_xdr.c -o server
	gcc RPC_Client.c RPC_xFile_clnt.c RPC_xFile_xdr.c -o client
server:
	rpcgen RPC_xFile.x
	gcc RPC_Server.c RPC_xFile_svc.c RPC_xFile_xdr.c -o server
client:
	rpcgen RPC_xFile.x
	gcc RPC_Client.c RPC_xFile_clnt.c RPC_xFile_xdr.c -o client
debug:
	rpcgen RPC_xFile.x
	gcc -g RPC_Server.c RPC_xFile_svc.c RPC_xFile_xdr.c -o server
	gcc -g RPC_Client.c RPC_xFile_clnt.c RPC_xFile_xdr.c -o client
clean:
	rm RPC_xFile_svc.c RPC_xFile_xdr.c RPC_xFile_clnt.c RPC_xFile.h server client
