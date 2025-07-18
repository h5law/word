CC = gcc

CFLAGS  := -Isrc/ \
		   $(shell pkg-config --cflags openssl) \
		   $(shell pkg-config --cflags limbo_sqlite3) \
		   -DTB_IMPL -DTB_LIB_OPTS
		   # $(shell pkg-config --cflags nac) \

LDFLAGS := $(shell pkg-config --libs openssl) \
		   $(shell pkg-config --libs limbo_sqlite3) \
		   # $(shell pkg-config --libs ncurses) \

SRC_DIR = src
TEST_DIR = tests

SOURCES = $(wildcard $(SRC_DIR)/*.c)
OBJECTS = $(SOURCES:.c=.o)

TEST_SOURCES = $(wildcard $(TEST_DIR)/*.c)
TEST_OBJECTS = $(TEST_SOURCES:.c=.o)

.PHONY: clean build test demo

clean:
	rm -vf $(OBJECTS) *.o */*.o */**/*.o
	rm -vf $(OBJECTS:.o=.d) *.d */*.d */**/*.d
	rm -vf $(TEST_OBJECTS)
	rm -vf words

$(SRC_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) -MD $(CFLAGS) -c -o $@ $<

build: $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o words $(OBJECTS)

$(TEST_DIR)/test_hotp.o:
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(TEST_DIR)/test_hotp.o $(TEST_DIR)/test_hotp.c $(SRC_DIR)/hotp.c $(SRC_DIR)/totp.c

$(TEST_DIR)/test_totp.o:
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(TEST_DIR)/test_totp.o $(TEST_DIR)/test_totp.c $(SRC_DIR)/hotp.c $(SRC_DIR)/totp.c

test: $(TEST_OBJECTS)
	exec $(TEST_DIR)/test_totp.o
	exec $(TEST_DIR)/test_hotp.o
