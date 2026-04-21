#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>


#define IP   0x0A832033
#define PORT 9025


int main() {
	struct sockaddr_in addr = {
		.sin_family = AF_INET,
		.sin_port   = htons(PORT),
		.sin_addr 	= htonl(IP)
	};

	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	connect(sockfd, (struct sockaddr *)&addr, sizeof(addr));
	send(sockfd, "From client :>", 4, 0);

	char buf[256];
	recv(sockfd, buf, 256, 0);

	printf("%s", buf);
	close(sockfd);

	return 0;
}
