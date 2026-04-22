#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>


#define PORT htons(9025)


typedef void exch_t(int sockfd, char *tx, int tx_size, char *rx, int rx_size);

exch_t exch_ashost;
void exch_ashost(int sockfd, char *tx, int tx_size, char *rx, int rx_maxsize) {
	send(sockfd, tx, tx_size,    0);
	recv(sockfd, rx, rx_maxsize, 0);
}

exch_t exch_asclient;
void exch_asclient(int sockfd, char *tx, int tx_size, char *rx, int rx_maxsize) {
	recv(sockfd, rx, rx_maxsize, 0);
	send(sockfd, tx, tx_size,    0);
}


int initialise_game(exch_t **exch) {
	int self_sockfd = socket(AF_INET, SOCK_STREAM, 0);

	do {
		size_t ans_length;
		char   *ans = NULL;
		printf("[h]ost game or [c]onnect to game? ");
		getline(&ans, &ans_length, stdin);

		if (ans[1] != '\n')
			continue;

		switch (ans[0]) {
			case 'h':
				*exch = &exch_ashost;

				int one = 1;
				setsockopt(self_sockfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &one, sizeof(one));

				struct sockaddr_in self_addr = {
					.sin_family = AF_INET,
					.sin_port   = PORT,
					.sin_addr   = INADDR_ANY
				};
				bind(self_sockfd, (struct sockaddr *) &self_addr, sizeof(self_addr));
				listen(self_sockfd, 1);

				printf("Waiting for opponent...\n");
				int sockfd = accept(self_sockfd, NULL, NULL);

				printf("Connected!\n");
				return sockfd;

			case 'c':
				*exch = &exch_asclient;

				char ip_buf[16];
				printf("Enter opponent IP: ");
				scanf("%s", ip_buf);
				fgetc(stdin);

				struct sockaddr_in peer_addr = {
					.sin_family = AF_INET,
					.sin_port   = PORT
				};
				if (inet_pton(AF_INET, ip_buf, &peer_addr.sin_addr) != 1) {
					printf("Invalid IP - please try again.\n");
					continue;
				}

				printf("Connecting...\n");
				connect(self_sockfd, (struct sockaddr *) &peer_addr, sizeof(peer_addr));

				printf("Connected!\n");
				return self_sockfd;

			default:
				continue;
		}

	} while (true);
}
