/*
 * test-helper.c
 *
 * This file contains helper methods for some test functions.
 *
 * (C) 2004,2024 Peter Conrad <conrad@unix-ag.uni-kl.de>
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pkctypes.h>
#include <qsort.h>

uword *key2i;

#define MAX_SORTED 0x400000

int main(int argc, char **argv) {
int i, num = 0;

    if (argc == 1) {
	/* Read numbers from stdin */
	key2i = (uword*) malloc(MAX_SORTED * sizeof(uword));
	while (scanf("%d", &i) == 1 && num < MAX_SORTED) {
	    key2i[num++] = i;
	}
    } else if (argc == 3 && !strcmp("-n", argv[1])) {
	/* Generate argv[2] numbers and write them to STDERR */
	num = atoi(argv[2]);
	if (num < 0) { num = 0; }
	if (num > MAX_SORTED) { num = MAX_SORTED; }
	key2i = (uword*) malloc(num * sizeof(uword));
	for (i = 0; i < num; i++) {
	    key2i[i] = random() % (num + 1);
	    fprintf(stderr, "%d\n", key2i[i]);
	}
    } else {
	fprintf(stderr, "Usage: %s [-n number]\n", argv[0]);
	exit(1);
    }

    qSort(0, num - 1);

    for (i = 0; i < num; i++) {
        printf("%d\n", key2i[i]);
    }

    free(key2i);

    exit(0);
}
