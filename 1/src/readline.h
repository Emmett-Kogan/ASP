#pragma once

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
