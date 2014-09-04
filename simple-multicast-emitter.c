#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

void diep(char *str) {
	fprintf(stderr, "[-] ");
	perror(str);
	exit(EXIT_FAILURE);
}
 
int main(int argc, char *argv[]) {
	struct in_addr local;
	struct sockaddr_in group;
	char *buffer = "Hello World !\n";
	ssize_t length;
	int fd;
	
	if(argc < 2) {
		fprintf(stderr, "Usage: %s local-ip\n", argv[0]);
		return 1;
	}
	
	printf("[+] building socket\n");
	
	if((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
		diep("socket");
	
	memset((char *) &group, 0, sizeof(group));
	group.sin_family = AF_INET;
	group.sin_addr.s_addr = inet_addr("226.1.1.1");
	group.sin_port = htons(4321);
	
	printf("[+] building multicast settings\n");
	local.s_addr = inet_addr(argv[1]);
	if(setsockopt(fd, IPPROTO_IP, IP_MULTICAST_IF, (char*)&local, sizeof(local)) < 0)
		diep("setsockopt: IP_MULTICAST_IF");
	
	printf("[+] sending data\n");
	if((length = sendto(fd, buffer, strlen(buffer), 0, (struct sockaddr*) &group, sizeof(group))) < 0)
		perror("sendto");
	
	printf("[+] %ld bytes sent\n", length);
	
	return 0;
}
