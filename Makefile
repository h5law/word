CC = clang

CFLAGS  = -Isrc/ \
 $(shell pkg-config --cflags openssl) \
 $(pkg-config --cflags limbo_sqlite3) \
 $(dialog-config --cflags)

LDFLAGS  = $(shell pkg-config --libs openssl) \
 $(pkg-config --libs limbo_sqlite3) \
 $(dialog-config --libs)

SRC_DIR = src
TEST_DIR = tests

SOURCES = $(wildcard $(SRC_DIR)/*.c)
OBJECTS = $(SOURCES:.c=.o)

TEST_SOURCES = $(wildcard $(TEST_DIR)/*.c)
TEST_OBJECTS = $(TEST_SOURCES:.c=.o)

.PHONY: clean build test demo

clean:
	rm -vf $(OBJECTS)
	rm -vf $(TEST_OBJECTS)

$(SRC_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

build: $(OBJECTS)
	$(CC) $(CFLAGS) -o $(SRC_DIR)/words $(OBJECTS)

$(TEST_DIR)/test_hotp.o:
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(TEST_DIR)/test_hotp $(TEST_DIR)/test_hotp.c $(SRC_DIR)/hotp.c $(SRC_DIR)/totp.c

$(TEST_DIR)/test_totp.o:
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(TEST_DIR)/test_totp $(TEST_DIR)/test_totp.c $(SRC_DIR)/hotp.c $(SRC_DIR)/totp.c

test: $(TEST_OBJECTS)
	exec $(TEST_DIR)/test_hotp
	exec $(TEST_DIR)/test_totp
