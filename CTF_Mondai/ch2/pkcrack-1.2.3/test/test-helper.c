/*
 * test-helper.c
 *
 * This file contains helper methods for some test functions.
 *
 * (C) 2004,2024 Peter Conrad <conrad@unix-ag.uni-kl.de>
 */

#include <stdio.h>
#include <stdlib.h>
#include "pkcrack.h"
#include "keystuff.h"
#include "test-helper.h"

byte    *plaintext = NULL, *ciphertext = NULL;
uword   *key2i = NULL;
int     numKey2s = 0;

uword   *realKey2is = NULL;

/* Allocate memory: length bytes for plaintext and ciphertext, length uwords
 * for realKey2is, and KEY2SPACE uwords for key2i
 */
static void setup_memory(int length) {
    if (plaintext)  { free(plaintext);  plaintext  = NULL; }
    if (ciphertext) { free(ciphertext); ciphertext = NULL; }
    if (realKey2is) { free(realKey2is); realKey2is = NULL; }

    if (!key2i) {
        key2i = (uword*) malloc(KEY2SPACE * sizeof(*key2i));
        if (!key2i) {
            fprintf(stderr, "malloc for key2i failed!\n");
            exit(1);
        }
    }

    plaintext = (byte*) malloc(length * sizeof(*plaintext));
    if (!plaintext) {
        fprintf(stderr, "malloc for plaintext failed!\n");
        exit(1);
    }

    ciphertext = (byte*) malloc(length * sizeof(*ciphertext));
    if (!ciphertext) {
        fprintf(stderr, "malloc for ciphertext failed!\n");
        exit(1);
    }

    realKey2is = (uword*) malloc(length * sizeof(*realKey2is));
    if (!realKey2is) {
        fprintf(stderr, "malloc for realKey2is failed!\n");
        exit(1);
    }
}

/* Walk through plaintext, encrypting it to ciphertext and saving key2 values
 * in realKey2is. setup_memory(length) should have been called before that.
 */
static void calculate_keys(int length) {
int i;

    initkeys();
    for (i = 0; i < length; i++) {
        realKey2is[i] = key2;
        ciphertext[i] = plaintext[i] ^ key3;
        updateKeys(plaintext[i]);
    }
}

/* Create a predefined plaintext / ciphertext / key2i sequence of length 612. */
void setup_deterministic_test(void) {
int i;

    setup_memory(DETERMINISTIC_LENGTH);
    for (i = 0; i < DETERMINISTIC_LENGTH; i++) {
        plaintext[i] = (i < 100) ? 0
                                 : (i < 356) ? i - 100
                                             : DETERMINISTIC_LENGTH - i;
    }
    calculate_keys(DETERMINISTIC_LENGTH);
}

/* Generate pseudorandom plaintext of the given length, then calculate the
 * ciphertext and the key2i sequence.
 */
void setup_random_test(int length) {
int i;

    setup_memory(length);
    for (i = 0; i < length; i++) {
        plaintext[i] = random();
    }
    calculate_keys(length);
}
