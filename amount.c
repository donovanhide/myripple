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

void format_amount(amount_t *a,char *result,unsigned long *length,bool debug){
	mpq_canonicalize(a->sum);
	mpz_set_ui(a->factor,5);
	mpq_get_num(a->num,a->sum);
	mpq_get_den(a->den,a->sum);
	mp_bitcnt_t m = mpz_scan1(a->den,0);
	mp_bitcnt_t n = mpz_remove(a->den,a->den,a->factor);
	if (m>n){
		mpz_ui_pow_ui(a->factor,5,m-n);
		mpz_mul(a->num,a->num,a->factor);
	}else{
		mpz_mul_2exp(a->num,a->num,n-m);
	}
	mpz_get_str(a->buf, 10, a->num);
	size_t len = strlen(a->buf);
	if (m==0){
		// integral
		*length=gmp_snprintf(result,a->max_length,"%s",a->buf);
	}else if (m>=len){
		// less than one
		*length=gmp_snprintf(result,a->max_length,"0.%0*s",m-len+1,a->buf);
	}else{
		// greater than one
		*length=gmp_snprintf(result,a->max_length,"%.*s.%.*s",len-n,a->buf,n,a->buf+len-n);
	}
	if (debug){
		*length=gmp_snprintf(result,a->max_length,"%s %d %d %d %Qu",result,m,n,len,a->sum);
	}
}