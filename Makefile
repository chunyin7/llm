SRC:=$(wildcard src/**/*.c)

train_vocab:
	gcc ${SRC} run/train_vocab.c -o train_vocab -Iinclude -fsanitize=address
	./train_vocab
	rm train_vocab

clean:
	rm train_vocab
