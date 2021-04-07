CC=gcc
SLL=ar cr
CLIENT_SRC=src/client/client.c src/client/arg_parser.c src/client/settings.c
WORKER_SRC=src/server/server.c
MASTER_SRC=src/master.c
SLAVEIN := ..
TESTSRC := test/fs_test.c test/server_table_test.c test/file_exec_test.c test/transmission_test.c
LIBC := lib/comm.c lib/PP/packet.c lib/interface.c lib/PP/transmission.c lib/PP/transmission_serializer.c
LIBO := comm.o packet.o interface.o transmission.o transmission_serializer.o
IN := $(foreach src, $(LIBC), $(src)) src/fs/fs.c src/server_table.c src/server/file_exec.c src/worker_admin.c src/balance.c src/server/boot.c src/server/argument.c
INCLUDE := lib
THIRDP_INCLUDE=include
MACROS=-DDEBUG -DLOG -DWORKER_TKN=$(WORKER_TKN) -DMASTER_TKN=$(MASTER_TKN)
TEST_MACROS=-DDEBUG -DLOG -DWORKER_TKN=1 -DMASTER_TKN=2
CFLAGS=$(MACROS) $(foreach include, $(INCLUDE), -I$(include)) -I$(THIRDP_INCLUDE) -lpthread
TEST_CFLAGS=$(TEST_MACROS) $(foreach include, $(INCLUDE), -I$(include)) -I$(THIRDP_INCLUDE) -lpthread

MASTER_BUILD=$(CC) -o master $(MASTER_SRC) $(foreach i, $(IN), $(i)) $(CFLAGS)
WORKER_BUILD=$(CC) -o worker $(WORKER_SRC) $(foreach i, $(IN), $(i)) $(CFLAGS)
CLIENT_BUILD=$(CC) -o pekar $(CLIENT_SRC) $(foreach i, $(IN), $(i)) $(CFLAGS) && mkdir bin && mv pekar bin
OBJECTS_BUILD=$(foreach src, $(LIBC), $(CC) -c $(src) $(foreach include, $(INCLUDE), -I$(include)) &&) mkdir include

LIB=libpekardrive.a

.PHONY: all test clean

all: $(MASTER_SRC) $(WORKER_SRC) $(CLIENT_SRC) $(foreach i, $(IN), $(i)) ; \
    $(MASTER_BUILD) && $(WORKER_BUILD) && $(CLIENT_BUILD)

master: $(MASTER_SRC) $(foreach i, $(IN), $(i)) ; \
    $(MASTER_BUILD)

worker: $(WORKER_SRC) $(foreach i, $(IN), $(i)) ; \
    $(WORKER_BUILD)

client: $(CLIENT_SRC) $(foreach i, $(IN), $(i)) ; \
    $(CLIENT_BUILD)

lib: objects ; \
    $(SLL) $(LIB) $(foreach obj, $(LIBO), $(obj)) && mv $(LIB) include && rm $(foreach obj, $(LIBO), $(obj))

objects: $(foreach src, $(LIBC), $(src)) ; \
    $(OBJECTS_BUILD)

test: $(foreach src, $(TESTSRC), $(src)) $(foreach i, $(IN), $(i)) ; \
    $(foreach t, $(TESTSRC), $(CC) $(t) $(foreach i, $(IN), $(i)) $(TEST_CFLAGS) && ./a.out &&) rm a.out

clean: ; \
    rm -rf master worker bin
