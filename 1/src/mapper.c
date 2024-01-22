#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include "readline.h"

int main() {
	int index, i;
	char buffer[24];

	while(1) {
		index = 0;
		memset(buffer, 0, 24);

		if(readline(buffer, 24) == 0 || !buffer[0] || buffer[0] == '\n') break;

		for (i = 0; i < 6; i++) printf("%c", buffer[i]);
		for (i = 8; i < 24 && buffer[i] != ')'; i++) printf("%c", buffer[i]);
		
		switch (buffer[6]) {
		case 'P':
			printf(",50)\n");
			break;
		case 'L':
			printf(",20)\n");
			break;
		case 'D':
			printf(",-10)\n");
			break;
		case 'C':
			printf(",30)\n");
			break;
		case 'S':
			printf(",40)\n");
			break;
		default:
			perror("Illegal action");
		}
	}

	return 0;
}
