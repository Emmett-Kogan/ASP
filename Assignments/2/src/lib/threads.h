// Author: Emmett Kogan
// Last modified: 2/11/24

#ifndef THREADS_H
#define THREADS_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "FIFO.h"

typedef struct tuple_t {
    int id;
    char topic[33];
    int value;
} tuple_t;

typedef struct node_t {
    tuple_t data;
    struct node_t *next;
} node_t;

static void *reducer(void *args) {
    char buffer[33];
    node_t *head = NULL;
    int count = 0;

	while(1) {
		memset(buffer, 0, 33);
        FIFO_pop((FIFO_t *) args, buffer);

        if (buffer[0] == '\n')
            break;

        char *_id, *topic, *_value;
        char *tmp = buffer;
        const char del[4] = "(),\0";

        _id = strtok_r(buffer, del, &tmp);
        topic = strtok_r(0, del, &tmp);
        _value = strtok_r(0, del, &tmp);

        int id = atoi(_id);
        int value = atoi(_value);

        // Traverse list and update node if matching topic found
        node_t *node = head;
        int flag = 1, l1 = 0;
        while (topic[l1]) l1++;

        while(node) {
            // Compare topic to node->data.topic
            int l2 = 0;
            while (node->data.topic[l2]) l2++;

            // printf("in: %s\tnode: %s\nl1: %d, l2: %d\n", topic, node->data.topic,l1,l2);

            // If topics match update node and clear flag
            if (l1 == l2 && !strncmp(topic, node->data.topic, l1)) {
                node->data.value += value;
                flag = 0;
                break;
            }

            node = node->next;
        }

        // If no match found make new node
        if (flag) {
            node_t *n = (node_t *) calloc(1, sizeof(node_t));
            n->data.id = id;
            memcpy(n->data.topic, topic, l1);
            n->data.value = value;

            // Push at front of list
            n->next = head;
            head = n;
        }
    }

    // Need to update this to traverse the LL
    node_t *node = head;
    while(node) {
        printf("(%04d,%s,%d)\n", node->data.id, node->data.topic, node->data.value);
        node = node->next;
    }

    return 0;
}

#endif
