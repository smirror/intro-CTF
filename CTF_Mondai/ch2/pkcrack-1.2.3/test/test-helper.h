/*
 * test-helper.h
 *
 * Header file for test helper functions.
 *
 * (C) 2004 Peter Conrad <conrad@unix-ag.uni-kl.de>
 */

#define DETERMINISTIC_LENGTH    612

extern uword   *realKey2is;

extern void setup_deterministic_test(void);
extern void setup_random_test(int length);
