#include "amount.h"

void decode_amount(amount_t* a,mpq_t dest,const char *b){
	uint64_t v = 0;
	for (int i = 0; i < 8; i++) {
  		v = (v << 8) | (uint8_t)(b[i]);
	}
	int exp;
	if ((v>>63)==1){
		// Non-native currency
		mpz_set_ui(a->num,v&0x3FFFFFFFFFFFFF);
		exp=((v>>54)&0xFF)-97;
	}else{
		// Native currency
		mpz_set_ui(a->num,v&0x3FFFFFFFFFFFFFFF);
		exp=-6;
	}
	if (((v>>62)&1)==0){
		// Negative
		mpz_neg(a->num,a->num);
	}
 	mpz_ui_pow_ui(a->den,10,abs(exp));
	if (exp>0){
		mpz_mul(a->num,a->num,a->den);
		mpq_set_z(dest,a->num);
	}else{
		mpq_set_num(dest,a->num);
		mpq_set_den(dest,a->den);
	}
}

amount_t* new_amount(int max_length){
	amount_t *a=malloc(sizeof(amount_t));
	mpq_inits(a->sum,a->addend,NULL);
	mpz_inits(a->factor,a->num,a->den,NULL);
	a->buf=malloc(sizeof(char)*max_length);
	if (a->buf==NULL){
		return NULL;
	}
	a->max_length=max_length;
	return a;
}

void free_amount(amount_t *a){
	mpq_clears(a->sum,a->addend,NULL);
	mpz_clears(a->factor,a->num,a->den,NULL);
	free(a->buf);
	free(a);
}

void set_amount(amount_t *a,const char *b){
	decode_amount(a,a->sum,b);
}

void add_amount(amount_t *a,const char *b){
	decode_amount(a,a->addend,b);
	mpq_add(a->sum,a->sum,a->addend);
}

void clear_amount(amount_t *a){
	mpq_set_ui(a->sum,0,1);
}

// Thanks to Paul Zimmerman, David Gillies and Marco Bodrato 
// on the GMP mailing list for help on this.
void format_amount(amount_t *a,char *result,unsigned long *length){
	mpq_canonicalize(a->sum);
	mpq_get_num(a->num,a->sum);
	mpq_get_den(a->den,a->sum);
	if (mpz_cmp_ui(a->den,1)==0){
		// Integral
		mpz_get_str(result,10,a->num);
		*length=strlen(result);
		return;
	}
	bool negative=(mpz_sgn(a->num)==-1);
	mpz_abs(a->num,a->num);
	bool greater_than_one=false;
	if (mpz_cmp(a->num,a->den)>0){
		greater_than_one=true;
	}
	mp_bitcnt_t m = mpz_scan1(a->den,0);
	mpz_set_ui(a->factor,5); 
	mp_bitcnt_t n = mpz_remove(a->den,a->den,a->factor);
	// Or
	// size_t n = mpz_sizeinbase (a->den, 2) - m;
	// n = ((n + 1) * 3 + n / 68) / 7;
	if (m>n){
		mpz_ui_pow_ui(a->factor,5,m-n);
		mpz_mul(a->num,a->num,a->factor);
	}else{
		mpz_mul_2exp(a->num,a->num,n-m);
	}
	if (greater_than_one){
		// greater than one
		mpz_get_str(a->buf,10,a->num);
		size_t len=strlen(a->buf);
		size_t offset=(m>n)?len-m:len-n;
		char *fmt=negative?"-%.*s.%s":"%.*s.%s";
		*length=gmp_snprintf(result,a->max_length,fmt,offset,a->buf,a->buf+offset);
	}else{
		// less than one
		size_t offset=(m>n)?m:n;
		char *fmt=negative?"-0.%0*Zu":"0.%0*Zu";
		*length=gmp_snprintf(result,a->max_length,fmt,offset,a->num);
	}
}