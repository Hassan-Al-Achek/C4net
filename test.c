#include <stdio.h>
#include <string.h>
#include <stdlib.h>
// 192.168.0.103
// strlen(IP) - 1 = '3'
// strlen(IP) - 2 = '1'
void getByteIP(char *IP, char *lastByte){
	int index = 0;
	char *temp = IP;

	for(int i = 10; i <= 12; i++){
		lastByte[index] = *(temp + i);
		index++;
	}
	lastByte[3]='\0';
	IP[10] = '\0';
}

int splitIPs(char *hosts, char **seperatedIPs){
	

	char lastByte[4];
	int index = 0;
	char *hostname= strdup(hosts);
       	while(strchr(hostname, '-') != NULL){
		
		seperatedIPs[index] = strsep(&hostname, "-");
		
		if(index == 0){
			getByteIP(seperatedIPs[0], lastByte);
			index++;
			seperatedIPs[index] = lastByte;	
		}
		index++;
	}
	seperatedIPs[index] = hostname;
	index++;
	return index;	
}


void getListOfSensorIPs(char **seperatedIPs, int numOfIPs){
	char first3Bytes[]="";
	char temp[]="";
	for(int i=0; i<numOfIPs; i++){
		if(i == 0) {
		      strcpy(first3Bytes, seperatedIPs[0]);	      
		} else {
			/*strcpy(temp, seperatedIPs[i]);
			strcat(seperatedIPs[i], first3Bytes);
			strcat(seperatedIPs[i], temp);
			printf("%s\n", seperatedIPs[i]);
			*/
		}
	}
}


void main(int argc, char *argv[]){
	char *hostname;
	char *seperatedIPs[10];
	int numOfIPs;
	if(argc == 2){
		hostname= strdup(argv[1]);
		printf("%s\n", hostname);
		numOfIPs = splitIPs(hostname, seperatedIPs);
		for (int i=0; i < numOfIPs; i++)
			printf("IP: %s\n", seperatedIPs[i]);
		getListOfSensorIPs(seperatedIPs, numOfIPs);
		for (int i = 0; i < numOfIPs; i++)
			printf("IP: %s\n", seperatedIPs[i]);
		
	}
}
