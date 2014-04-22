#ifndef AMOUNT_H_
#define AMOUNT_H_
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <gmp.h>

typedef struct {
	mpq_t sum;
	mpq_t addend;
	mpz_t factor;
	mpz_t num;
	mpz_t den;
	char* buf;
	int max_length;
}amount_t;

amount_t* new_amount(int max_length);
void free_amount(amount_t *a);
void set_amount(amount_t *a,const char *b);
void add_amount(amount_t *a,const char *b);
void clear_amount(amount_t *a);
void format_amount(amount_t *a,char *result,unsigned long *length);

#endif  // AMOUNT_H_
