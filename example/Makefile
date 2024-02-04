
#指定编译器
CC = g++

#编译选项
CFLAGS = -fPIC -shared -std=c++11 
$(warning CFLAGS is $(CFLAGS))

#头文件路径
INCLUDE := -I/home/timer/Fast-DDS/install/include   #FastDDS的头文件目录
INCLUDE += -I/home/timer/Fast-DDS                   #cmw目录的上一级目录

#找出当前目录下，所有的源文件
SRCS := $(shell find ../* -type f | grep '\.cpp' | grep -v './writer.cpp' | grep -v './reader.cpp' | grep -v './test.*\.cpp')
$(warning SRCS is ${SRCS})

#确定cpp源文件对应的目标文件
OBJS := $(patsubst %.cpp, %.o , $(filter %.cpp, $(SRCS)))
$(warning OBJS is ${OBJS})


#第三方动态库的路径
ThirdPartyLibPath := /home/timer/Fast-DDS/install/lib #FastDDS的动态库目录

#第三方动态库
ThirdPartyLib := -lfastrtps -lfastcdr -lfoonathan_memory-0.7.3 
ThirdPartyLib += -lpthread
ThirdPartyLib += -luuid

# .PHONY:libcmw.so
# libcmw.so: $(SRCS)
# 	$(CC) $(INCLUDE) -o $@ $^ $(CFLAGS)  -L$(ThirdPartyLibPath) $(ThirdPartyLib) -Wl,-rpath=$(ThirdPartyLibPath)


writer: $(SRCS) test_writer.cpp
	$(CC) $(INCLUDE) -o $@.out $^   -L$(ThirdPartyLibPath) $(ThirdPartyLib) -Wl,-rpath=$(ThirdPartyLibPath) 


reader: $(SRCS) test_reader.cpp
	$(CC) $(INCLUDE) -o $@.out $^   -L$(ThirdPartyLibPath) $(ThirdPartyLib) -Wl,-rpath=$(ThirdPartyLibPath)

test_serialize: $(SRCS) test_serialize.cpp
	$(CC) $(INCLUDE) -o $@.out $^   -L$(ThirdPartyLibPath) $(ThirdPartyLib) -Wl,-rpath=$(ThirdPartyLibPath)

test_node_manager: $(SRCS) test_node_manager.cpp
	$(CC) $(INCLUDE) -o $@.out $^   -L$(ThirdPartyLibPath) $(ThirdPartyLib) -Wl,-rpath=$(ThirdPartyLibPath)

test_channel_manager: $(SRCS) test_channel_manager.cpp
	$(CC) $(INCLUDE) -o $@.out $^   -L$(ThirdPartyLibPath) $(ThirdPartyLib) -Wl,-rpath=$(ThirdPartyLibPath)

test_topology_manager: $(SRCS) test_topology_manager.cpp
	$(CC) $(INCLUDE) -o $@.out $^   -L$(ThirdPartyLibPath) $(ThirdPartyLib) -Wl,-rpath=$(ThirdPartyLibPath)
	
test_publisher: $(SRCS) test_publisher.cpp
	$(CC) $(INCLUDE) -o $@.out $^   -L$(ThirdPartyLibPath) $(ThirdPartyLib) -Wl,-rpath=$(ThirdPartyLibPath)

test_subscriber: $(SRCS) test_subscriber.cpp
	$(CC) $(INCLUDE) -o $@.out $^   -L$(ThirdPartyLibPath) $(ThirdPartyLib) -Wl,-rpath=$(ThirdPartyLibPath)
.PHONY:clean
clean:
	rm -rf *.out