#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define buffSize 2048

void main (int argc, char *argv[]){
	char *hostname, *temp;
	unsigned int port;
	int fdSocket;
	struct hostent *hostNameInfo;
	struct sockaddr_in serverAddr;
	int isConnected;
	ssize_t isReceived;
	char *receivedData;

	if(argc == 2){
		temp = strdup(argv[1]);
		hostname = strsep(&temp, ":");
		if (temp){
			port = atoi(temp);
		}
	} else {
		printf("[!] Usage: %s hostname:portnum\n", argv[0]);
		exit(1);
	}
	fprintf(stderr, "[+] Connecting to %s:%d\n", hostname, port);
	
	// Create Socket File Descriptor
	fdSocket = socket(AF_INET, SOCK_STREAM, 0);
	printf("[+] Socket fd: %d\n", fdSocket);
	// Resolve Hostname -> IP address
	printf("[+] Hostname: %s\n", hostname);	
	hostNameInfo = gethostbyname(hostname);
	printf("[+] IP Address: %s\n", inet_ntoa(*((struct in_addr *) hostNameInfo->h_addr_list[0])));

	
	bzero(&serverAddr, sizeof(serverAddr));
	
	serverAddr.sin_family = AF_INET;
	// Convert From Host Byte Order To Network Byte Order
	serverAddr.sin_port = htons(port);

	// Get IP Address From hostNameInfo Structure
	bcopy(hostNameInfo->h_addr_list[0], &serverAddr.sin_addr, sizeof(serverAddr.sin_addr));
	
	// Connect To Server (The Sensor)
	isConnected = connect(fdSocket, (struct sockaddr*) &serverAddr, sizeof(serverAddr));
	
	if (isConnected == 0 ){
		printf("[+] Successfully Connected To %s:%d\n", inet_ntoa(*((struct in_addr *)hostNameInfo->h_addr_list[0])), port);
	} else {
		perror("[-] Connection Error | Reason");
		close(fdSocket);
		exit(1);
	}

	// Receive Data From The Server (Sensor)
	isReceived = recv(fdSocket, receivedData, buffSize * sizeof(char), 0);
	if (isReceived == -1){
		perror("[-] No Data Received\n");
	} else {
		printf("[<-+] Data Successfully Received From %s:%d\n", inet_ntoa(*((struct in_addr *)hostNameInfo->h_addr_list[0])), port);
		printf("[+] Data: %s\n", receivedData);

	}

	close(fdSocket);
	printf("[~] Connection Closed\n");

}
