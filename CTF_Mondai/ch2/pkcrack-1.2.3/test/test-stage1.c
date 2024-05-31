/*
 * test-stage1.c
 *
 * This file contains some tests for functions in stage1.c.
 *
 * (C) 2004,2024 Peter Conrad <conrad@unix-ag.uni-kl.de>
 */

#include <stdio.h>
#include <stdlib.h>
#include "pkcrack.h"
#include "test-helper.h"
#include "stage1.h"
#include "mktmptbl.h"
#include "crc.h"

#define RANDOM_LENGTH 2000

static void do_test_stage0(int length) {
uword key2 = realKey2is[length - 1];
int i;

    generate1stSetOfKey2s(length - 1);
    for (i = 0; i < numKey2s; i++) {
       if (key2i[i] == key2) {
           return;
       }
    }
    fprintf(stderr, "test of generate1stSetOfKey2s failed for length %d\n",
            length);
    exit(1);
}

static void do_test_stage1(int length) {
int i, found, l = length;
uword key2;

    generate1stSetOfKey2s(--l);
    while (l > 2) {
        if (numKey2s > l) {
            key2i[l / 2] = realKey2is[l];
            numKey2s = l > 1701 ? l : 1701;
        }
        reduceKey2s(l);
        key2 = realKey2is[--l];
        for (i = found = 0; i < numKey2s && !found; i++) {
            found |= (key2 == key2i[i]);
        }
        if (!found) {
            fprintf(stderr,
                    "test of reduceKey2s failed for length %d at position %d\n",
                    length, l);
            exit(1);
        }
    }
}

static void test_stage0() {
    setup_deterministic_test();
    do_test_stage0(DETERMINISTIC_LENGTH);
    setup_random_test(RANDOM_LENGTH);
    do_test_stage0(RANDOM_LENGTH);
    setup_random_test(RANDOM_LENGTH * 2);
    do_test_stage0(RANDOM_LENGTH * 2);
    setup_random_test(RANDOM_LENGTH * 3);
    do_test_stage0(RANDOM_LENGTH * 3);
}

static void test_stage1() {
    setup_deterministic_test();
    do_test_stage1(DETERMINISTIC_LENGTH);
    setup_random_test(RANDOM_LENGTH);
    do_test_stage1(RANDOM_LENGTH);
    setup_random_test(RANDOM_LENGTH * 2);
    do_test_stage1(RANDOM_LENGTH * 2);
    setup_random_test(RANDOM_LENGTH * 3);
    do_test_stage1(RANDOM_LENGTH * 3);
}

int main(int argc, char **argv) {
    preCompTemp();
    mkCrcTab();
    initMulTab();
    test_stage0();
    test_stage1();
    exit(0);
}
