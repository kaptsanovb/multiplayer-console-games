#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>


#define PORT htons(9025)


void waitdots_cleanup(void *done) {
	puts("!");
	setlinebuf(stdout);
	*((bool *) done) = true;
}

void *waitdots_helper(void *done) {
	pthread_cleanup_push(&waitdots_cleanup, done);
	setbuf(stdout, NULL);
	*((bool *) done) = false;

	putchar('.');
	while (true) {
		for (int i = 0; i < 2; ++i) {
			usleep(500000);
			putchar('.');
		}

		usleep(500000);
		printf("\x1B[2D\x1B[0J");
	}

	pthread_cleanup_pop(1);
	return NULL;
}

typedef struct {
	pthread_t thread;
	bool      done;
} DotsThread;

DotsThread waitdots(char *s) {
	fputs(s, stdout);

	DotsThread dthread = {
		.done = false
	};
	pthread_create(&dthread.thread, NULL, &waitdots_helper, &dthread.done);

	return dthread;
}

void dotsthread_cancel(DotsThread dthread) {
	pthread_cancel(dthread.thread);
	while (!dthread.done);
}


void exch(int sockfd, char *tx, int tx_size, char *rx, int rx_maxsize) {
	send(sockfd, tx, tx_size, 0);

	if (recv(sockfd, rx, rx_maxsize, MSG_DONTWAIT) == -1) {
		DotsThread thread = waitdots("Waiting for opponent");
		recv(sockfd, rx, rx_maxsize, 0);
		dotsthread_cancel(thread);
	}
}

int initialise_game() {
	int self_sockfd = socket(AF_INET, SOCK_STREAM, 0);

	do {
		size_t ans_length;
		char   *ans = NULL;
		printf("[h]ost game, [c]onnect to peer, or connect to [s]erver? ");
		getline(&ans, &ans_length, stdin);

		if (ans[1] != '\n')
			continue;

		switch (ans[0]) {
			case 'h':
				int one = 1;
				setsockopt(self_sockfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &one, sizeof(one));

				struct sockaddr_in self_addr = {
					.sin_family = AF_INET,
					.sin_port   = PORT,
					.sin_addr   = INADDR_ANY
				};
				bind(self_sockfd, (struct sockaddr *) &self_addr, sizeof(self_addr));
				listen(self_sockfd, 1);

				DotsThread dthreadh = waitdots("Waiting for opponent");
				int sockfd = accept(self_sockfd, NULL, NULL);
				dotsthread_cancel(dthreadh);
				puts("Opponent found!");

				return sockfd;
				break;

			case 'c':
				char ip_buf[16];
				printf("Enter opponent IP: ");
				scanf("%s", ip_buf);
				fgetc(stdin);

				struct sockaddr_in peer_addr = {
					.sin_family = AF_INET,
					.sin_port   = PORT
				};
				if (inet_pton(AF_INET, ip_buf, &peer_addr.sin_addr) != 1) {
					puts("Invalid IP - please try again.");
					continue;
				}

				DotsThread dthreadc = waitdots("Connecting");
				connect(self_sockfd, (struct sockaddr *) &peer_addr, sizeof(peer_addr));
				dotsthread_cancel(dthreadc);
				puts("Connected!");

				dthreadc = waitdots("Waiting for opponent");
				recv(self_sockfd, NULL, 1, 0);
				dotsthread_cancel(dthreadc);
				puts("Found opponent!");

				return self_sockfd;
				break;

			case 's':
				char ip_buf[16];
				printf("Enter opponent IP: ");
				scanf("%s", ip_buf);
				fgetc(stdin);

				struct sockaddr_in server_addr = {
					.sin_family = AF_INET,
					.sin_port   = PORT
				};
				if (inet_pton(AF_INET, ip_buf, &server_addr.sin_addr) != 1) {
					puts("Invalid IP - please try again.");
					continue;
				}

				DotsThread dthreads = waitdots("Connecting");
				connect(self_sockfd, (struct sockaddr *) &server_addr, sizeof(server_addr));
				dotsthread_cancel(dthreads);
				puts("Connected!");

				return self_sockfd;
				break;

			default:
				continue;
		}

	} while (true);
}

void exch_server(int peer1_sockfd, char *rx1, int peer2_sockfd, char *rx2) {
	bool ready1 = false;
	bool ready2 = false;
	while (
		!(ready1 || (ready1 = recv(peer1_sockfd, rx1, 1, 0) != -1)) &&
		!(ready2 || (ready2 = recv(peer2_sockfd, rx2, 1, 0) != -1))
	);

	send(peer1_sockfd, &rx2, 1, 0);
	send(peer2_sockfd, &rx1, 1, 0);
}
