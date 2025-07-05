TOKEN:=token/token.c
MAP:=token/tokenmap/tokenmap.c
ARR:=arr/array.c
TEST:=test.c

token_test:
	gcc ${TOKEN} ${ARR} ${MAP} ${TEST} -o tok -fsanitize=address
	./tok
	rm tok
