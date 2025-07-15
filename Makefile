SRC:=$(wildcard src/**/*.c)

train_vocab:
	gcc ${SRC} run/train_vocab.c -o train_vocab -Iinclude
	./train_vocab
	rm train_vocab

tokenize:
	gcc ${SRC} run/tokenize.c -o tokenize -Iinclude
	./tokenize ${file}
	rm tokenize

clean:
	rm train_vocab
	rm tokenize

.PHONY: clean
