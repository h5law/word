CC=clang

CFLAGS=$(shell pkg-config --cflags openssl)
CFLAGS=$(shell pkg-config --libs openssl)

SOURCES=hotp.c totp.c
TESTS=otp_test.c

OBJECTS=hotp.o totp.o
TEST_OBJECTS=otp_test.o

.PHONY: clean build test demo

clean:
	rm -vf $(OBJECTS) $(TEST_OBJECTS) test

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

build: $(OBJECTS)

test: $(SOURCES) $(TESTS)
	# $(LD) $(LDFLAGS) -o test $(TEST_OBJECTS) $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o test $(TESTS) $(SOURCES)
	./test

