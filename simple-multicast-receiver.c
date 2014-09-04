#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>

int reuse = 1;

void diep(char *str) {
	fprintf(stderr, "[-] ");
	perror(str);
	exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]) {
	struct sockaddr_in local;
	struct ip_mreq group;
	char buffer[1024];
	ssize_t dlen;
	int fd;
	
	if(argc < 2) {
		fprintf(stderr, "Usage: %s local-ip\n", argv[0]);
		return 1;
	}

	printf("[+] building socket\n");
	if((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
		diep("socket");
	
	printf("[+] setting up socket\n");
	if(setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (char *) &reuse, sizeof(reuse)) < 0)
		diep("setsockopt: SO_REUSEADDR");
	 
	memset((char *) &local, 0, sizeof(local));
	local.sin_family = AF_INET;
	local.sin_port = htons(4321);
	local.sin_addr.s_addr = INADDR_ANY;
	
	printf("[+] binding socket\n");
	if(bind(fd, (struct sockaddr*) &local, sizeof(local)))
		diep("bind");
	 
	group.imr_multiaddr.s_addr = inet_addr("226.1.1.1");
	group.imr_interface.s_addr = inet_addr(argv[1]);
	
	printf("[+] joining multicast group\n");
	if(setsockopt(fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *) &group, sizeof(group)) < 0)
		diep("setsockopt: IP_ADD_MEMBERSHIP");
	
	printf("[+] waiting data\n");
	while((dlen = read(fd, buffer, sizeof(buffer))) > 0) {
		printf("[+] %ld bytes read\n", dlen);
		
		buffer[dlen] = '\0';
		printf("[+] %s", buffer);
	}
	
	return 0;
}
