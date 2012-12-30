/* Simple TCP client, for Linux.
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
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#define CONNECT_PORT    80
#define CONNECT_ADDR    "localhost"

void diep(char *str) {
	perror(str);
	exit(EXIT_FAILURE);
}

int working(int sockfd);

int main(void) {
	int sockfd;
	struct sockaddr_in addr_remote;
	struct hostent *hent;

	/* Creating client socket */
	addr_remote.sin_family      = AF_INET;
	addr_remote.sin_port        = htons(CONNECT_PORT);
	
	/* dns resolution */
	if((hent = gethostbyname(CONNECT_ADDR)) == NULL)
		diep("[-] gethostbyname");
		
	memcpy(&addr_remote.sin_addr, hent->h_addr_list[0], hent->h_length);
	
	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
		diep("[-] socket");

	/* connecting */
	if(connect(sockfd, (const struct sockaddr *) &addr_remote, sizeof(addr_remote)) < 0)
		diep("[-] connect");

	printf("[+] Connected\n");
	working(sockfd);
	
	return 0;
}

int working(int sockfd) {
	char buffer[256];
	int length;
	
	/* http request example */
	/* strcpy(buffer, "GET / HTTP/1.0\r\n\r\n"); */
	
	strcpy(buffer, "Hello world !\n");
	if(send(sockfd, buffer, strlen(buffer), 0) < 0)
		diep("[-] send");
	
	printf("[+] Message sent\n");
	
	if((length = recv(sockfd, buffer, sizeof(buffer), 0)) < 0)
		perror("[-] read");
	
	buffer[length] = '\0';
	printf("[+] Buffer: %s\n", buffer);
	
	return 0;
}
