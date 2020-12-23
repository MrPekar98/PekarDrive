CC=gcc
WORKER_SRC=src/server/server.c
MASTER_SRC=src/master.c
SLAVEIN := ..
TESTSRC := test/fs_test.c test/server_table_test.c
IN := lib/comm.c lib/packet.c src/fs/fs.c src/server_table.c
INCLUDE := lib
CFLAGS=-DDEBUG -DLOG $(foreach include, $(INCLUDE), -I$(include)) -lpthread

.PHONY: all test clean

master: $(MASTER_SRC) $(foreach i, $(IN), $(i)) ; \
    $(CC) -o master $(MASTER_SRC) $(foreach i, $(IN), $(i)) $(CFLAGS)

worker: $(WORKER_SRC) $(foreach i, $(IN), $(i)) ; \
    $(CC) -o worker $(WORKER_SRC) $(foreach i, $(IN), $(i)) $(CFLAGS)

test: $(foreach src, $(TESTSRC), $(src)) $(foreach i, $(IN), $(i)) ; \
    $(foreach t, $(TESTSRC), $(CC) $(t) $(foreach inc, $(INCLUDE), -I$(inc)) \
        $(foreach i, $(IN), $(i)) && ./a.out && rm a.out)

clean: ; \
    rm master worker
