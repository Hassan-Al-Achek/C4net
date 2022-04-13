/*
 * Description: TCP Server 
 * Auther: Hassan Al Achek
 * Compilation: gcc etape2.c -o etape2
 * Last Edit: 12 Apr 2022
 * Note: Local Server Can't Be Accessed From
 * Outside the LAN, If You Want To Do That
 * Setup Iptable with Port Forwarding
 * */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define port 4442
#define buffSize 2048

void main(int argc, char* argv[]){
	// Deafult IP Address
	const char *ip = "127.0.0.1";
	struct sockaddr_in serverAddr, clientAddr;
	int fdSocket, clientSocket;
	int isBinded;
	ssize_t isReceived;
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
	printf("[+] Start Listning On %s:%d\n", ip, port);
	
	while(1){
		// Extract First Socket From The Queue
		clientSocket = accept(fdSocket, (struct sockaddr *)&clientAddr, &sizeOfSIn);
		
		printf("\n[+] Connection Received From: %s:%d\n", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));

		printf("[+->] Sending Data: %s", dataToSend);
		send(clientSocket, dataToSend, strlen(dataToSend) + 1, 0);
		
		// Receive Data From Client
		isReceived = recv(clientSocket, receivedData, buffSize, 0);
		
		if(isReceived != -1)
			receivedData [isReceived] = '\0';
		
		printf("[<-+] Data Received From %s:%d\n", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));
		printf("[<-+] Data: %s\n", receivedData);
			
		// printf("[+->] Sending Data: %s\n", dataToSend);
		// send(clientSocket, dataToSend, strlen(dataToSend), 0);
	
		close(clientSocket);
	}

	close(fdSocket);
	printf("[~] Connection Closed\n");	
}
