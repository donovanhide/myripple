#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <mysql.h>
#include "amount.h"

const int MAX_LENGTH=255;

// reusable functions
my_bool init(UDF_INIT *initid, UDF_ARGS *args, char *msg,char *usage){
	if (args->arg_count!=1 && args->arg_count!=2){
		strcpy(msg,usage);
		return 1;
	}
	args->arg_type[0] = STRING_RESULT;
	if (args->arg_count==2){
		args->arg_type[1] = INT_RESULT;
	}
	initid->max_length=MAX_LENGTH;
	amount_t *amount=new_amount(MAX_LENGTH);
	if (amount==NULL){
		strcpy(msg,"could not allocate memory");
		return 1;
	}
	initid->ptr=(char*)amount;
	return 0;
}

my_bool de_init(UDF_INIT *initid){
	free_amount((amount_t*)initid->ptr);
	return 0;
}

char* output(UDF_INIT *initid, UDF_ARGS *args, char *result, unsigned long *length,my_bool *is_null,my_bool *error){
	amount_t* amount=(amount_t*)initid->ptr;
	bool debug=false;
	if (args->arg_count==2){
		debug=*((long long*)args->args[1])>0;
	}
	format_amount(amount,result,length,debug);
	return result;
}

// SQL UDF definitions
my_bool amount_init(UDF_INIT *initid, UDF_ARGS *args, char *msg){
	return init(initid,args,msg,"amount(8 bytes of amount,[debug int])");
}

my_bool amount_deinit(UDF_INIT *initid){
	return de_init(initid);
}

char* amount(UDF_INIT *initid, UDF_ARGS *args, char *result, unsigned long *length,my_bool *is_null,my_bool *error){
	amount_t* amount=(amount_t*)initid->ptr;
	set_amount(amount,args->args[0]);
	return output(initid,args,result,length,is_null,error);
}

my_bool sum_amount_init(UDF_INIT *initid, UDF_ARGS *args, char *msg){
	return init(initid,args,msg,"sum_amount(8 bytes of amount,[debug int])");
}

my_bool sum_amount_deinit(UDF_INIT *initid){
	return de_init(initid);
}

void sum_amount_clear(UDF_INIT *initid, char *is_null, char *error){
	amount_t* amount=(amount_t*)initid->ptr;
	clear_amount(amount);
}

void sum_amount_add(UDF_INIT *initid, UDF_ARGS *args,char *is_null, char *error){
	amount_t* amount=(amount_t*)initid->ptr;
	add_amount(amount,args->args[0]);
}

char* sum_amount(UDF_INIT *initid, UDF_ARGS *args, char *result, unsigned long *length,my_bool *is_null,my_bool *error){
	return output(initid,args,result,length,is_null,error);
}

