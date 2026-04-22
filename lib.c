#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>


#define PORT htons(9025)


typedef struct {
	bool host;
	int  sockfd;
} Connection;


Connection initialise_game() {
	Connection connection;
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
				connection.host = true;

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
				connection.sockfd = accept(self_sockfd, NULL, NULL);

				printf("Connected!\n");
				return connection;

			case 'c':
				connection.host = false;
				connection.sockfd = self_sockfd;

				char ip_buf[16];
				printf("Enter opponent IP: ");
				scanf("%s", ip_buf);

				struct sockaddr_in peer_addr = {
					.sin_family = AF_INET,
					.sin_port   = PORT
				};
				if (inet_pton(AF_INET, ip_buf, &peer_addr.sin_addr) != 1) {
					printf("Invalid IP - please try again.\n");
					continue;
				}

				printf("Connecting...\n");
				connect(connection.sockfd, (struct sockaddr *) &peer_addr, sizeof(peer_addr));

				printf("Connected!\n");
				return connection;

			default:
				continue;
		}

	} while (true);
}
