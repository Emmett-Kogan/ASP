// Author: Emmett Kogan
// Last modified: 2/4/24

#pragma once
#include <unistd.h>

int readline(char *buffer, int max) {
	int count = 0;
	while(--max) {
		count++;
		read(0, buffer, 1);
		if (*(buffer++) == '\n') {
			*buffer = '\0';
			return count;
		}
	}

	return count;
}

int writeline(int pfd, char *buffer) {
	int i = 0;
	while (buffer[i]) write(pfd, buffer+(i++), 1);
	return i;
}
