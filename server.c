#include "lib.c"


#define PORT 9025


int main() {
	int one = 1;

	struct sockaddr_in addr = {
		.sin_family = AF_INET,
		.sin_port   = htons(PORT),
		.sin_addr   = INADDR_ANY
	};

	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &one, sizeof(one));
	bind(sockfd, (struct sockaddr *) &addr, sizeof(addr));
	listen(sockfd, 1);

	struct sockaddr_in peer_addr;
	int waiting_sockfd = accept(sockfd, NULL, NULL);
	int peer_sockfd    = accept(sockfd, NULL, NULL);

	while (true) {
		exch_server(waiting_sockfd, NULL, peer_sockfd, NULL);
		exch_server(waiting_sockfd, NULL, peer_sockfd, NULL);

		char ch1, ch2;
		exch_server(waiting_sockfd, &ch1, peer_sockfd, &ch2);

		if (!(ch1 == 'y' && ch2 == 'y'))
			break;
	}

	close(sockfd);

	char buf[256];
	recv(peer_sockfd, buf, 256, 0);
	printf("%s\n", buf);
	send(peer_sockfd, "test!!", 6, 0);

	close(peer_sockfd);
	return 0;
}
