#include "lib.c"


int main() {
	int sockfd = initialise_game();
	int self_score = 0;
	int peer_score = 0;

	do {
		printf("Score: You %d - %d Peer\n", self_score, peer_score);
		char choice;
		do {
			size_t ans_length;
			char   *ans = NULL;
			printf("[r]ock, [p]aper, or [s]cissors? ");
			getline(&ans, &ans_length, stdin);

			if (ans_length < 2 || ans[1] != '\n' || (ans[0] != 'r' && ans[0] != 'p' && ans[0] != 's'))
				continue;

			choice = ans[0];
			break;
		} while (true);


		char rx[1];
		exch(sockfd, "d", 1, rx, 1);
		if (rx[0] != 'd')
				return 1;

		exch(sockfd, &choice, 1, rx, 1);
		if ((choice == 'r' && rx[0] == 'r')
		 || (choice == 'p' && rx[0] == 'p')
		 || (choice == 's' && rx[0] == 's')
		)
			printf("Draw!\n");
		else if ((choice == 'r' && rx[0] == 's')
		 || (choice == 'p' && rx[0] == 'r')
		 || (choice == 's' && rx[0] == 'p')
		) {
			printf("You win!\n");
			++self_score;
		} else if ((choice == 'r' && rx[0] == 'p')
		 || (choice == 'p' && rx[0] == 's')
		 || (choice == 's' && rx[0] == 'r')
		) {
			printf("You lose :<\n");
			++peer_score;
		} else
			return 1;

		do {
			size_t ans_length;
			char   *ans = NULL;
			printf("Play again (y/n)? ");
			getline(&ans, &ans_length, stdin);

			if (ans_length < 2 || ans[1] != '\n' || (ans[0] != 'n' && ans[0] != 'y'))
				continue;

			choice = ans[0];
			break;
		} while (true);

		exch(sockfd, &choice, 1, rx, 1);
		if (choice == 'n' || rx[0] == 'n') {
			printf("Game over!\n");
			close(sockfd);
			break;
		}

	} while (true);

	return 0;
}
