CFLAGS := -std=c99 -pthread -Wall -Wextra -Werror -Wno-unused-parameter -Wno-unused-function

ifdef DEBUG
	CFLAGS += -g
endif

# Depencies for compiling
DEP_FILES=\
	src/http.o\
	src/server.o\
	src/requests/request.o\
	src/utils/memory.o\
	src/utils/hashtable.o\
	src/utils/json.o\

# Locations of object files
OBJ_FILES=\
	src/http.o\
	src/server.o\
	src/requests/request.o\
	src/utils/memory.o\
	src/utils/hashtable.o\
	src/utils/json.o\

all: server


server: $(DEP_FILES)
	gcc src/main.c -o server $(OBJ_FILES) $(CFLAGS)

src/http.o: src/http.c
	gcc $(CFLAGS) -fPIC -c src/http.c -o src/http.o

src/server.o: src/server.c
	gcc $(CFLAGS) -fPIC -c src/server.c -o src/server.o

src/requests/request.o: src/requests/request.c
	gcc $(CFLAGS) -fPIC -c src/requests/request.c -o src/requests/request.o

src/utils/memory.o: src/utils/memory.c
	gcc $(CFLAGS) -fPIC -c src/utils/memory.c -o src/utils/memory.o

src/utils/hashtable.o: src/utils/hashtable.c
	gcc $(CFLAGS) -fPIC -c src/utils/hashtable.c -o src/utils/hashtable.o

src/utils/json.o: src/utils/json.c
	gcc $(CFLAGS) -fPIC -c src/utils/json.c -o src/utils/json.o

clean:
	find . -type f -name "*.o" | xargs rm -v

.PHONY: server
