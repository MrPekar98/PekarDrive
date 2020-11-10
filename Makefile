CC=gcc
SRC=src/server.c
TESTSRC := test/fs_test.c
IN := src/comm.c src/fs/fs.c src/server_table.c
CFLAGS=-DDEBUG

.PHONY: all test clean

scam: $(SRC) $(foreach i, $(IN), $(i)) ; \
    $(CC) -o PekarServer $(SRC) $(foreach i, $(IN), $(i)) $(CFLAGS)

test: ; \
    $(foreach t, $(TESTSRC),  $(CC) $(t) $(foreach i, $(IN), $(i)) && ./a.out && rm a.out)

clean: ; \
    rm PekarServer
