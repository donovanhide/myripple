#Borrowed from https://github.com/joaocosta/lib_mysqludf_ta
#Things you might need to change depending on your environment
MYSQL_PLUGIN_DIR=`mysql_config --plugindir`
CC=gcc
CFLAGS=-Wall -std=c99 -m64 -O2 `mysql_config --include` -c 
LD=ld 
LDFLAGS=-shared -lgmp -lc

#Things you shouldn't need to change if you just mean to build this
SOURCES=amount.c myripple.c
OBJECTS=$(SOURCES:.c=.o)
BINARY=myripple.so

all: $(SOURCES) $(BINARY)

$(BINARY): $(OBJECTS)
	$(LD) $(LDFLAGS) $(OBJECTS) -o $@

.c.o:
	$(CC) $(CFLAGS) $< -o $@

install: $(BINARY)
	cp $(BINARY) $(MYSQL_PLUGIN_DIR)

check:
	mysql -t < test.sql

tidy:
	uncrustify -c ./uncrustify.cfg --no-backup --replace $(SOURCES)

clean:
	rm -f $(BINARY) *.o