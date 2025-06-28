TOKEN:=token/token.c
HASHMAP:=hashmap/hashmap.c
TEST:=test.c

token_test:
	gcc ${TOKEN} ${HASHMAP} ${TEST} -o tok -fsanitize=address
	./tok
	rm tok
