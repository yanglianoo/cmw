#指定编译器
CC = g++

#编译选项
CFLAGS = -fPIC -shared -std=c++11 
$(warning CFLAGS is $(CFLAGS))

#头文件路径
INCLUDE := -I/home/timer/Fast-DDS/install/include
INCLUDE += -I/home/timer/Fast-DDS
INCLUDE += -I.

#找出当前目录下，所有的源文件
SRCS := $(shell find ./* -type f | grep '\.cpp' | grep -v './writer.cpp' | grep -v './reader.cpp' | grep -v './test.*\.cpp')
$(warning SRCS is ${SRCS})

#确定cpp源文件对应的目标文件
OBJS := $(patsubst %.cpp, %.o , $(filter %.cpp, $(SRCS)))
$(warning OBJS is ${OBJS})


#第三方动态库的路径
ThirdPartyLibPath := /home/timer/Fast-DDS/install/lib

#第三方动态库
ThirdPartyLib := -lfastrtps -lfastcdr -lfoonathan_memory-0.7.3 
ThirdPartyLib += -lpthread
ThirdPartyLib += -luuid

# .PHONY:libcmw.so
# libcmw.so: $(SRCS)
# 	$(CC) $(INCLUDE) -o $@ $^ $(CFLAGS)  -L$(ThirdPartyLibPath) $(ThirdPartyLib) -Wl,-rpath=$(ThirdPartyLibPath)

.PHONY:writer
writer: $(SRCS) writer.cpp
	$(CC) $(INCLUDE) -o $@ $^   -L$(ThirdPartyLibPath) $(ThirdPartyLib) -Wl,-rpath=$(ThirdPartyLibPath)

.PHONY:reader 
reader: $(SRCS) reader.cpp
	$(CC) $(INCLUDE) -o $@ $^   -L$(ThirdPartyLibPath) $(ThirdPartyLib) -Wl,-rpath=$(ThirdPartyLibPath)