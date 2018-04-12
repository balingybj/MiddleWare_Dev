
## 获取编译环境是否为arm
SUBARCH := $(shell echo $(CC) | cut -d- -f1)

CFLAGS = $(INCL) -g  -Wall -rdynamic -lm

MAKE = make
RM = rm -rf
MV = mv

TEST_ROOT = $(PWD)

BIN = $(TEST_ROOT)/bin

ifeq ($(SUBARCH), arm)
	HOST = arm-linux
	RANLIB = arm-linux-gnueabihf-ranlib
LIB_PATH = -L./lib
LIBS = -lrt -ldl -lpthread -lreadline -lncurses
else
	HOST = x86_64-linux
	CC = cc
	CFLAGS += -m32
LIBS = -lrt -ldl -lpthread -lreadline -lncurses
endif

SRC_PATH =  $(TEST_ROOT)/src
INC_PATH = $(TEST_ROOT)/inc

SRC_FILES = $(wildcard $(addsuffix /*.c,$(SRC_PATH)))
OBJS = $(patsubst %.c,%.o,$(SRC_FILES))

TARGET = test_log

INCL = -I$(INC_PATH)

all : $(TARGET)

test_log : $(OBJS)
	@$(CC) $(OBJS)  $(LIBS) $(LIB_PATH) -o $(TARGET) $(CFLAGS)
	$(MV) $(TARGET) $(BIN)

clean:
	rm *.o


