
## 获取编译环境是否为arm
SUBARCH := $(shell echo $(CC) | cut -d- -f1)

#使用C99标准
CFLAGS = $(INCL) -g  -Wall -rdynamic -lm -std=c99

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
LIBS = -lrt -ldl -lpthread -lreadline -lncurses -lcommon
else
	CC = cc
	CFLAGS += -m32
LIB_PATH = -L$(TEST_ROOT)/../pub/lib
LIBS = -lrt -ldl -lpthread -lreadline -lncurses -lcommon
endif

SRC_PATH =  $(TEST_ROOT)/src
INC_PATH = -I$(TEST_ROOT)/inc \
			-I$(TEST_ROOT)/../common/inc

SRC_FILES = $(wildcard $(addsuffix /*.c,$(SRC_PATH)))

JSON_OBJS =   $(SRC_PATH)/json_test.o
READ_OBJS =   $(SRC_PATH)/readline_test.o
TH1_OBJS =   $(SRC_PATH)/thread1_test.o
TH2_OBJS = $(SRC_PATH)/thread2_test.o 


TARGET = test_read test_json test_thread1 test_thread2
#TARGET = test_read

INCL = $(INC_PATH)

all : $(TARGET)

test_read : $(READ_OBJS)
	@$(CC) $(READ_OBJS)   $(LIBS) $(LIB_PATH)  -o test_read $(CFLAGS)
	$(MV) test_read $(BIN)

test_json : $(JSON_OBJS)
	@$(CC) $(JSON_OBJS)     $(LIBS) $(LIB_PATH) -o test_json $(CFLAGS)
	$(MV) test_json $(BIN)

test_thread1 : $(TH1_OBJS)
	@$(CC) $(TH1_OBJS)  $(LIBS) $(LIB_PATH) -o test_thread1 $(CFLAGS)
	$(MV) test_thread1 $(BIN)

test_thread2 : $(TH2_OBJS)
	@$(CC) $(TH2_OBJS)  $(LIBS) $(LIB_PATH) -o test_thread2 $(CFLAGS)
	$(MV) test_thread2 $(BIN)

## 清除.o及可执行文件  ，$$i ，首先取得Makefile中$i值，然后在shell中$($(i))
clean:
	@for i in $(SRC_PATH); do \
	echo "clean .o in $$i..."; \
		(cd $$i; $(RM) *.o); done; \
	cd $(BIN); $(RM) $(TARGET); cd $(TEST_ROOT)



