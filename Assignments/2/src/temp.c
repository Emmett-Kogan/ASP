#pragma once

#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include <string>
#include <vector>
#include <tuple>

#include "wrline.h"
#include "FIFO.h"

extern "C" static void *reducer(void *args) {
    char buffer[32];
    vector<tuple<int, string, int>> tuples;

	while(1) {
		memset(buffer, 0, 32);
        FIFO_pop((FIFO_t *) args, buffer);

        if (buffer[0] == '\n')
            break;

		string s(buffer);

		// Get index of second comma
		int index;
		for (index = 7; index < s.length(); index++)
			if (buffer[index] == ',') break;

		// Get id, topic and value
		int id = stoi(s.substr(1,4));
		string topic = s.substr(6, index-6);
		int value = stoi(s.substr(index+1,s.length()-index+1));

		// Updating tuples and clear flag
		int flag = 1;
		for (tuple<int,string,int> &t : tuples) {
			if (id == get<0>(t)) {
				if (!topic.compare(get<1>(t))) {
					get<2>(t) += value;
					flag = 0;
				}
			}
		}

		// If flag not cleared make new tuple
		if (flag) tuples.push_back(make_tuple(id, topic, value));
	}

	for (tuple<int,string,int> t : tuples)
		printf("(%04d,%s,%d)\n", get<0>(t), get<1>(t).c_str(), get<2>(t));

	return 0;
}
