
## 获取编译环境是否为arm
SUBARCH := $(shell echo $(CC) | cut -d- -f1)

CFLAGS = $(INCL) -g  -Wall -rdynamic -lm

MAKE = make
RM = rm -rf
MV = mv

TEST_ROOT = $(PWD)

## 生成目标文件路径
BIN = $(TEST_ROOT)/../bin

ifeq ($(SUBARCH), arm)
	HOST = arm-linux
	RANLIB = arm-linux-gnueabihf-ranlib
LIB_PATH = -L$(TEST_ROOT)/../pub/lib
LIBS = -lrt -ldl -lpthread -lreadline -lncurses
else
	HOST = x86_64-linux
	CC = cc
	CFLAGS += -m32
LIBS = -lrt -ldl -lpthread -lreadline 
endif

SRC_PATH =  $(TEST_ROOT)/src
INC_PATH = $(TEST_ROOT)/inc

SRC_FILES = $(wildcard $(addsuffix /*.c,$(SRC_PATH)))
OBJS = $(patsubst %.c,%.o,$(SRC_FILES))

TARGET = test_log

INCL = -I$(INC_PATH)

all : $(TARGET)

$(TARGET) : $(OBJS)
	@$(CC) $(OBJS)  $(LIBS) $(LIB_PATH) -o $(TARGET) $(CFLAGS)
	$(MV) $(TARGET) $(BIN)

## 清除.o及可执行文件  ，$$i ，首先取得Makefile中$i值，然后在shell中$($(i))
clean:
	@for i in $(SRC_PATH); do \
	echo "clean .o in $$i..."; \
		(cd $$i; $(RM) *.o); done; \
	cd $(BIN); $(RM) $(TARGET); cd $(TEST_ROOT)


