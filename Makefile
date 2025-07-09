SRC:=$(wildcard src/**/*.c)

token_test:
	gcc ${SRC} test.c -o tok -fsanitize=address -Iinclude
	./tok
	rm tok
