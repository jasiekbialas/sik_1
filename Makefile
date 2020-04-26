CC = gcc
CFLAGS = -Wall -gdwarf-2
TARGETS = testhttp_raw

all: $(TARGETS)

err.o: err.h

dynamic_string.o: dynamic_string.c dynamic_string.h err.h

regex_to_string.o: regex_to_string.h regex_to_string.c

parse_response.o: parse_response.c parse_response.h err.h dynamic_string.h regex_to_string.h

prepare.o: prepare.c prepare.h err.h regex_to_string.h

net.o: net.c net.h err.h parse_response.h

testhttp_raw.o: testhttp_raw.c dynamic_string.h prepare.h net.h err.h

testhttp_raw: testhttp_raw.o dynamic_string.o prepare.o net.o err.o parse_response.o regex_to_string.o

clean:
	rm -f *.o $(TARGETS) 
