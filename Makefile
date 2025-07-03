TOKEN:=token/token.c
HASHMAP:=hashmap/hashmap.c
ARR:=arr/array.c
TEST:=test.c

token_test:
	gcc ${TOKEN} ${ARR} ${HASHMAP} ${TEST} -o tok -fsanitize=address
	./tok
	rm tok
