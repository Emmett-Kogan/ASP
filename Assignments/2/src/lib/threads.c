#include "threads.h"

static void *reducer(void *args) {
    char buffer[32];
    tuple_t tuples[50];
    int count = 0;

	while(1) {
		memset(buffer, 0, 32);
        FIFO_pop((FIFO_t *) args, buffer);

        if (buffer[0] == '\n')
            break;


        char *id_s, *topic, *value_s;
        char *tmp = buffer;

        const char del[4] = "(,)";

        id_s = strtok_r(buffer, del, &tmp);
        topic = strtok_r(0, del, &tmp);
        value_s = strtok_r(0, del, &tmp);

        int id = atoi(id_s);
        int value = atoi(value_s);


        // have a flag to see if we need to append a new thing to our list
        // for each existing tuple
        // check if the current id matches, then check if the strings match
        // if the strings match update the value and clear the flag
        int flag;
        for (int i = 0; i < count; i++) {
            if (id == tuples[i].id) {
                // get length of each and compare the minimum
                int l1 = 0, l2 = 0;
                while(tuples[i].topic[l1] != '\0') l1++;
                while(topic[l2] != '\0') l2++;

                if (l1 == l2 && !strncmp(tuples[i].topic, topic, l1)) {
                    tuples[i].value +=
                }


            }
        }




        // otherwise if the flag is still set append the new tuple to the list

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
		if (flag) {
            tuples[count].id
            tuples[count].topic
            tuples[count].value
        }
	}

	for (tuple<int,string,int> t : tuples)
		printf("(%04d,%s,%d)\n", get<0>(t), get<1>(t).c_str(), get<2>(t));

	return 0;
}
