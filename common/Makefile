#编译共用库文件 .a文件
## 获取编译环境是否为arm
SUBARCH := $(shell echo $(CC) | cut -d- -f1)

#使用C99标准
CFLAGS = $(INCL) -g  -Wall -rdynamic -lm -std=c99 

MAKE = make
RM = rm -rf
MV = mv

#ar生成静态库文件
AR = ar rc


LIB_PATH =  

ifeq ($(SUBARCH), arm)
	HOST = arm-linux
	RANLIB = arm-linux-gnueabihf-ranlib
LIBS = -lrt -ldl -lpthread -lreadline -lncurses
else
	CC = cc
	CFLAGS += -m32
LIBS = -lrt -ldl -lpthread -lreadline -lncurses
endif

SRC_PATH =  ./src
INC_PATH = ./inc

SRC_FILES = $(wildcard $(addsuffix /*.c,$(SRC_PATH)))

OBJ_ = $(patsubst %.c,%.o,$(SRC_FILES))

#notdir  ->  去除变量中的路径
OBJ = $(notdir $(OBJ_))

#OBJ = cJson.o 

TARGET_PATH = ./../pub/lib

INCL = -I$(INC_PATH)

#多个.o文件生成一个.a文件
TARGET = libcommon.a

all:$(TARGET)

$(TARGET):$(OBJ)
	$(AR) $(TARGET) $(OBJ)
	$(MV) $(TARGET) $(TARGET_PATH)

$(OBJ):$(SRC_FILES)
	$(CC) $(LIBS) $(LIB_PATH) -c $(SRC_FILES) $(CFLAGS)
	#$(MV) 

	
clean:
	$(RM) $(OBJ)



