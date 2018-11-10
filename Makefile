CFLAGS=-std=c99
test_sexp: test_sexp.c sexp.c sexp.h
	$(CC) -o $@ $(filter %.c,$+)

clean:
	rm test_sexp
