#Borrowed from https://github.com/joaocosta/lib_mysqludf_ta
#Things you might need to change depending on your environment
MYSQL_PLUGIN_DIR=`mysql_config --plugindir`
CC=gcc
CFLAGS=-Wall -std=c99 -m64 -O2 `mysql_config --include` -fPIC -shared -lgmp 

#Things you shouldn't need to change if you just mean to build this
SOURCES=amount.c myripple.c
BINARY=myripple.so

all:
	$(CC) $(SOURCES) $(CFLAGS) -o $(BINARY)

install: $(BINARY)
	cp $(BINARY) $(MYSQL_PLUGIN_DIR)

check:
	mysql -t < test.sql

clean:
	rm -f $(BINARY) *.o