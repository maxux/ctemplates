/* Simple TCP server, for Linux, multi-threaded, with local specific
 * address to bind. Echo service for clients.
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <pthread.h>

#define LISTEN_PORT	9900
#define LISTEN_ADDR	"0.0.0.0"

typedef struct thread_data_t {
	int sockfd;
	struct sockaddr_in addr_client;
	pthread_t thread;
	
} thread_data_t;

int yes = 1;

void * working(void *thread);

void diep(char *str) {
	perror(str);
	exit(EXIT_FAILURE);
}

int main(void) {
	int sockfd, new_fd;
	struct sockaddr_in addr_listen, addr_client;
	socklen_t addr_client_len;
	char *client_ip;
	thread_data_t *thread_data;

	/* Creating Server Socket */
	addr_listen.sin_family      = AF_INET;
	addr_listen.sin_port        = htons(LISTEN_PORT);
	addr_listen.sin_addr.s_addr = inet_addr(LISTEN_ADDR);
	
	/* Init Client Socket Length */
	addr_client_len = sizeof(addr_client);
	
	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
		diep("[-] socket");

	if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
		diep("[-] setsockopt");

	if(bind(sockfd, (struct sockaddr*) &addr_listen, sizeof(addr_listen)) == -1)
		diep("[-] bind");

	if(listen(sockfd, 32) == -1)
		diep("[-] listen");

	/* Socket ready, waiting clients */
	while(1) {
		printf("[+] Waiting new connection...\n");
		
		if((new_fd = accept(sockfd, (struct sockaddr *)&addr_client, &addr_client_len)) == -1)
			perror("[-] accept");

		client_ip = inet_ntoa(addr_client.sin_addr);
		printf("[+] Connection from %s\n", client_ip);
		
		/* Warning: memory leak */
		thread_data = (thread_data_t*) malloc(sizeof(thread_data_t));
		
		thread_data->sockfd      = new_fd;
		thread_data->addr_client = addr_client;
		
		/* Starting new thread with our new client */
		if(pthread_create(&thread_data->thread, NULL, working, (void *) thread_data))
			diep("[-] pthread_create");
		
		if(pthread_detach(thread_data->thread))
			diep("[-] pthread_detach");
	}

	return 0;
}

void * working(void *thread) {
	char buffer[256];
	thread_data_t *thread_data = (thread_data_t*) thread;
	int length;
	
	printf("[+] Thread (fd %d) created\n", thread_data->sockfd);
	
	/* Echo Service */
	while((length = recv(thread_data->sockfd, buffer, sizeof(buffer), 0)) > 0) {
		buffer[length] = '\0';
		send(thread_data->sockfd, buffer, strlen(buffer), 0);
	}
		
	printf("[+] Thread (fd %d) closed\n", thread_data->sockfd);
	
	/* Clearing */
	close(thread_data->sockfd);
	free(thread);
	
	return 0;
}
