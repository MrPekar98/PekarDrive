CC=gcc
SLL=ar cr
CLIENT_SRC=src/client/client.c src/client/arg_parser.c src/client/settings.c
WORKER_SRC=src/server/server.c
MASTER_SRC=src/master.c
SLAVEIN := ..
TESTSRC := test/fs_test.c test/server_table_test.c test/file_exec_test.c test/transmission_test.c
TESTIN := test/transmission_networking.c test/pp_test_server.c
LIBC := lib/comm.c lib/PP/packet.c lib/interface.c lib/PP/transmission.c lib/PP/transmission_serializer.c lib/PP/pp.c
LIBO := comm.o packet.o interface.o transmission.o transmission_serializer.o
IN := $(foreach src, $(LIBC), $(src)) src/fs/fs.c src/server_table.c src/server/file_exec.c src/worker_admin.c src/balance.c src/server/boot.c src/server/argument.c src/util/logger.c
INCLUDE=include
THIRDP_INCLUDE=include
MACROS=-DVERBOSE_2 -DDEBUG -DWORKER_TKN=$(WORKER_TKN) -DMASTER_TKN=$(MASTER_TKN)
TEST_MACROS=-DDEBUG -DVERBOSE_2 -DWORKER_TKN=1 -DMASTER_TKN=2
CFLAGS=$(MACROS) -I$(INCLUDE) -lpthread
TEST_CFLAGS=$(TEST_MACROS) -I$(INCLUDE) -lpthread

MASTER_BUILD=$(CC) -o master $(MASTER_SRC) $(foreach i, $(IN), $(i)) $(CFLAGS)
WORKER_BUILD=$(CC) -o worker $(WORKER_SRC) $(foreach i, $(IN), $(i)) $(CFLAGS)
CLIENT_BUILD=$(CC) -o pekar $(CLIENT_SRC) $(foreach i, $(IN), $(i)) $(CFLAGS) && mkdir bin && mv pekar bin
#OBJECTS_BUILD=$(foreach src, $(LIBC), $(CC) -c $(src)  -I$(INCLUDE) &&)
OBJECTS_BUILD=$(CC) -c -I$(INCLUDE) $(foreach src, $(LIBC), $(src))

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

install: objects ; \
    $(SLL) $(LIB) $(foreach obj, $(LIBO), $(obj)) && mv $(LIB) lib && rm $(foreach obj, $(LIBO), $(obj))

objects: $(foreach src, $(LIBC), $(src)) ; \
    $(OBJECTS_BUILD)

test: $(foreach src, $(TESTSRC), $(src)) $(foreach i, $(IN), $(i)) $(foreach in, $(TESTIN), $(in)) ; \
    $(foreach t, $(TESTSRC), $(CC) $(t) $(foreach i, $(IN), $(i)) $(foreach in, $(TESTIN), $(in)) $(TEST_CFLAGS) && ./a.out &&) rm a.out

clean: ; \
    rm -rf master worker bin
