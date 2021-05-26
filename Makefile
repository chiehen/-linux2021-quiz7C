EXEC = pc-test
.PHONY: all
all: $(EXEC)

CC ?= gcc
CFLAGS = -std=gnu11 -Wall -g3
LDFLAGS = -lpthread

OBJS := \
	pc-test.o \
	ringbuffer.o

deps := $(OBJS:%.o=%.o.d)

%.o: %.c
	$(CC) $(CFLAGS) -c -MMD -MF .$@.d -o $@ $<

$(EXEC): $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

check: $(EXEC)
	@./$(EXEC)

clean:
	$(RM) $(EXEC) $(OBJS) $(deps)

-include $(deps)

#all: 
#	$(CC) -Wall -Wextra -o ringbuffer ringbuffer.c
#
#clean: 
#	rm -f ringbuffer
#
#indent: 
#	clang-format -i ringbuffer.c
