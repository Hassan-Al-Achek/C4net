#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define port 4444
#define buffSize 2048

void main(int argc, char* argv[]){
	const char *ip = "127.0.0.1";
	struct sockaddr_in serverAddr, clientAddr;
	int fdSocket, clientSocket;
	int isBinded;
	char receivedData[buffSize];
	char dataToSend[buffSize] = "~* Welcome To Server *~\n";
	// A Variable Contains The Size of sockaddr_in Structure
	// Used With accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen)
	// 								  ^
	// 								  |
	// 							-----------   
	int sizeOfSIn = sizeof(struct sockaddr_in);

	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(port);
	
	// Convert  Doted IP (i.e 127.0.0.1) to Network Byte Order
	inet_aton(ip, &serverAddr.sin_addr);
	
	fdSocket = socket(AF_INET, SOCK_STREAM, 0);

	isBinded = bind(fdSocket, (struct sockaddr *)&serverAddr, sizeof(struct sockaddr));
	if(isBinded == -1){
		perror("[-] Can't Bind | Reason");
		exit(1);
	}
	
	listen(fdSocket, 5);
	printf("[+] Start Listning At %s:%d\n", ip, port);
	
	while(1){
		// Extract First Socket From The Queue
		clientSocket = accept(fdSocket, (struct sockaddr *)&clientAddr, &sizeOfSIn);

		printf("[+->] Sending Data: %s\n", dataToSend);
		send(clientSocket, dataToSend, strlen(dataToSend), 0);
		
		printf("[+] Connection Received From %s:%d\n", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));
		// Receive Data From Client
		recv(clientSocket, receivedData, buffSize, 0);

		printf("[<-+] Data Received From %s:%d\n", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));
		printf("[<-+] Data: %s\n", receivedData);
	
		// printf("[+->] Sending Data: %s\n", dataToSend);
		// send(clientSocket, dataToSend, strlen(dataToSend), 0);
	
		close(clientSocket);
	}

	close(fdSocket);
	printf("[~] Connection Closed\n");	
}
