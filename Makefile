SRC:=$(wildcard src/**/*.c)
OUT_DIR:=bin

$(OUT_DIR):
	mkdir -p $@

train_vocab: $(OUT_DIR)
	gcc ${SRC} run/train_vocab.c -o train_vocab -Iinclude
	./train_vocab ${file} ${vocab_size}
	rm train_vocab

tokenize: $(OUT_DIR)
	gcc ${SRC} run/tokenize.c -o tokenize -Iinclude
	./tokenize ${file}
	rm tokenize

clean:
	rm train_vocab
	rm tokenize

.PHONY: clean
