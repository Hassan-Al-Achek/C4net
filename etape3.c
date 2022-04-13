/*
 * Description: TCP Server with Embeded Client:
 * 	CLient: Request Sensors Data
 * 	Server: Make Sensors Data Available To Clients
 * Auther: Hassan Al Achek
 * Compilation: gcc etape3.c -o etape3
 * Last Edit: 12 Apr 2022
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <signal.h>

#define port 4443
#define buffSize 2048
#define tempPort 8000
#define lumPort 8001
#define mouvPort 8002

// ProtoTypes
void getDataSensor(char *, int, char *);

// Global Variables
int fdSocket; // Sever Socket File Descriptor

float temperature(char *hostname, char *dataFromSensor)
{
	getDataSensor(hostname, tempPort, dataFromSensor);

	return atof(dataFromSensor);
}

float luminosite(char *hostname, char *dataFromSensor)
{
	getDataSensor(hostname, lumPort, dataFromSensor);

	return atof(dataFromSensor);
}

float mouvement(char *hostname, char *dataFromSensor)
{
	getDataSensor(hostname, mouvPort, dataFromSensor);

	return atof(dataFromSensor);
}

float averageTemp(char **hostname, char *dataFromSensor)
{
	int j;
	float avTemp = 0;
	for (j = 0; j < 3; j++)
	{
		avTemp += temperature(hostname[j], dataFromSensor);
	}
	avTemp /= 3;

	return avTemp;
}

float averageLum(char **hostname, char *dataFromSensor)
{
	int j;
	float avLum = 0;
	for (j = 0; j < 3; j++)
	{
		avLum += luminosite(hostname[j], dataFromSensor);
	}
	avLum /= 3;

	return avLum;
}

// There Is Only One Mouv Sensor
float averageMouv(char **hostname, char *dataFromSensor)
{
	int j = 2;
	float avMouv = 0;
	avMouv = mouvement(hostname[2], dataFromSensor);

	return avMouv;
}

void getDataSensor(char *hostname, int sensorPort, char *dataFromSensor)
{
	int sensorSocket;
	ssize_t isReceived;
	struct sockaddr_in sensorAddr;
	struct hostent *sensorInfo;

	// Dummy Data (The Sensor doesn't Accept To Give Me The Data Without It)
	char dummyData[] = "pleaseGiveMeCookie";

	// Client Part
	// Contact All Sensors (In My Case 1 Sensor)
	sensorSocket = socket(AF_INET, SOCK_STREAM, 0);
	sensorInfo = gethostbyname(hostname);

	printf("[+] Connecting To Sensor At: %s:%d\n", inet_ntoa(*((struct in_addr *)sensorInfo->h_addr_list[0])), sensorPort);

	bzero(&sensorAddr, sizeof(sensorAddr));

	sensorAddr.sin_family = AF_INET;
	sensorAddr.sin_port = htons(sensorPort);
	bcopy(sensorInfo->h_addr_list[0], &sensorAddr.sin_addr, sizeof(sensorAddr.sin_addr));
	// Connect To Sensor
	connect(sensorSocket, (struct sockaddr *)&sensorAddr, sizeof(sensorAddr));

	// Dummy Data For The Sensor
	send(sensorSocket, dummyData, strlen(dummyData), 0);
	// Receive Data From Sensor
	isReceived = recv(sensorSocket, dataFromSensor, buffSize, 0);
	if (isReceived != -1)
	{
		dataFromSensor[isReceived] = '\0';
	}
	printf("[<-+ Sensor] Data Received From The Sensor\n");
	printf("[<-+ Sensor] %s\n", dataFromSensor);

	close(sensorSocket);
	printf("[~] Connection Closed With Sensor\n\n");
}

// A Function To Handle Ctrl+c key
void handler(int fd)
{
	close(fd);
	printf("[~] Shut Down Server\n");
	exit(0);
}

void main(int argc, char *argv[])
{
	const char *ip = "127.0.0.1";
	struct sockaddr_in serverAddr, clientAddr;
	char *hostname, *temp;
	// Sensors IPs
	char *hostnames[20] = {"192.168.24.100", "192.168.24.102", "192.168.24.105"};
	unsigned int sensorPort;
	int clientSocket;
	int isBinded;
	char receivedData[buffSize], dataFromSensor[buffSize];
	char dataToSend[buffSize] = "~* Welcome To Server *~\n";
	float avTemp, avLum, avMouv;
	char *htmlToClient = (char *)malloc(400 * sizeof(char));
	// A Variable Contains The Size of sockaddr_in Structure
	// Used With accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen)
	// 								  ^
	// 								  |
	// 							-----------
	int sizeOfSIn = sizeof(struct sockaddr_in);

	// Handle Ctrl + c
	signal(SIGINT, handler);

	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(port);

	// Convert  Doted IP (i.e 127.0.0.1) to Network Byte Order
	inet_aton(ip, &serverAddr.sin_addr);

	fdSocket = socket(AF_INET, SOCK_STREAM, 0);

	isBinded = bind(fdSocket, (struct sockaddr *)&serverAddr, sizeof(struct sockaddr));

	if (isBinded == -1)
	{
		perror("[-] Can't Bind | Reason");
		exit(1);
	}

	listen(fdSocket, 5);
	printf("[+] Start Listning At %s:%d\n", ip, port);

	while (1)
	{

		// Extract First Socket From The Queue
		clientSocket = accept(fdSocket, (struct sockaddr *)&clientAddr, &sizeOfSIn);

		printf("\n[+->] Sending Data: %s", dataToSend);
		send(clientSocket, dataToSend, strlen(dataToSend), 0);

		printf("[+] Connection Received From: %s:%d\n", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));
		// Receive Data From Client
		recv(clientSocket, receivedData, buffSize, 0);

		printf("[<-+] Data Received From: %s:%d\n", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));
		printf("[<-+] Data: %s\n", receivedData);

		avTemp = averageTemp(hostnames, dataFromSensor);
		avLum = averageLum(hostnames, dataFromSensor);
		avMouv = averageMouv(hostnames, dataFromSensor);

		// Send Sensor Data To The Client
		printf("[+-> Client] Sending Temperature Data: %f\n", avTemp);
		printf("[+-> Client] Sending Luminosite Data: %f\n", avLum);
		printf("[+-> Client] Sending Mouvement Data: %f\n", avMouv);

		// Send Temperature Data
		char avCharTemp[100];
		char avCharLum[100];
		char avCharMouv[100];

		gcvt(avTemp, 7, avCharTemp);

		// send(clientSocket,avCharTemp, strlen(avCharTemp) + 1, 0);

		// Send Luminosité Data
		gcvt(avLum, 7, avCharLum);

		// send(clientSocket,avCharLum, strlen(avCharLum) + 1, 0);

		// Send Mouvement
		gcvt(avMouv, 7, avCharMouv);

		// send(clientSocket,avCharMouv, strlen(avCharMouv) + 1, 0);

		snprintf(htmlToClient, 400, "<pre>Temp:%s<br>Lum:%s<br>Mouvement:%s</pre>", avCharTemp, avCharLum, avCharMouv);
		// Bulk Send
		send(clientSocket, htmlToClient, strlen(htmlToClient) + 1, 0);

		free(htmlToClient);
		close(clientSocket);
	}
}
