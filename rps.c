#include "lib.c"


int main() {
	Connection connection = initialise_game();

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

	if (connection.host)
		send(connection.sockfd, "d", 1, 0);

	char rx;
	recv(connection.sockfd, &rx, 1, 0);
	if (rx != 'd')
		return 1;

	if (!connection.host)
		send(connection.sockfd, "d", 1, 0);

	if (connection.host)
		send(connection.sockfd, &choice, 1, 0);

	recv(connection.sockfd, &rx, 1, 0);
	if ((choice == 'r' && rx == 'r')
	 || (choice == 'p' && rx == 'p')
	 || (choice == 's' && rx == 's')
	)
		printf("Draw!\n");
	else if ((choice == 'r' && rx == 's')
	 || (choice == 'p' && rx == 'r')
	 || (choice == 's' && rx == 'p')
	)
		printf("You win!\n");
	else if ((choice == 'r' && rx == 'p')
	 || (choice == 'p' && rx == 's')
	 || (choice == 's' && rx == 'r')
	)
		printf("You lose :<\n");
	else
		return 1;

	if (!connection.host)
		send(connection.sockfd, &choice, 1, 0);

	return 0;
}
