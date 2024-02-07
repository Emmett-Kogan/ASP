#include <stdio.h>
#include <string.h>

int main() {
    char buff[100];

    char *id, *topic, *value;
    char *tmp = buff;

    scanf("%s", buff);

    const char del[4] = "(,)";

    id = strtok_r(buff, del, &tmp);
    topic = strtok_r(0, del, &tmp);
    value = strtok_r(0, del, &tmp);

    int l1 = 0;
    while(topic[l1] != '\0') l1++;

    printf("%s\n%s\n%s\n", id, topic, value);
    printf("Length of topic = %d\n", l1);

    return 0;
}
