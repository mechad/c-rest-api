cmake_minimum_required(VERSION 3.10)

project("C Rest Api"  VERSION 0.1.0)

add_library(crestapi STATIC
		# sources
		src/requests/request.c
		src/utils/hashtable.c
		src/utils/json.c
		src/utils/memory.c
		src/datatypes.c
		src/http.c
		src/server.c

		# headers
		src/requests/request.h
		src/utils/hashtable.h
		src/utils/json.h
		src/utils/memory.h
		src/datatypes.h
		src/http.h
		src/options.h
		src/server.h
		src/socketcon.h
	)
set(CMAKE_C_FLAGS
	"${CMAKE_C_FLAGS} \
	-std=c99 \
	-pthread \
	-Wall \
	-Wextra \
	-Werror \
	-Wno-unused-parameter \
	-Wno-unused-function"
	)

option(MODE "MyOption" "NORMAL")

if("${MODE}" STREQUAL "DEBUG")
	set(CMAKE_C_FLAGS
		"${CMAKE_C_FLAGS} \
		-O0 \
		-g \
		-DDEBUG"
	)
else()
	set(CMAKE_C_FLAGS
		"${CMAKE_C_FLAGS} \
		-O3 \
		-flto"
	)
endif()


