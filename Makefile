CC=gcc
SRC=src/server.c
SLAVESRC = slave/src/
SLAVEIN := ..
TESTSRC := test/fs_test.c
IN := src/comm.c src/fs/fs.c src/server_table.c
CFLAGS=-DDEBUG

.PHONY: all test clean

master: $(SRC) $(foreach i, $(IN), $(i)) ; \
    $(CC) -o master $(SRC) $(foreach i, $(IN), $(i)) $(CFLAGS)

slave: $(SLAVESRC) $(foreach i, $(SLAVEIN), $(i)) ; \
    $(CC) -o slave $(SLAVESRC) $(foreach i, $(SLAVEIN), $(i)) $(CLAGS)

test: ; \
    $(foreach t, $(TESTSRC),  $(CC) $(t) $(foreach i, $(IN), $(i)) && ./a.out && rm a.out)

clean: ; \
    rm master slave
