
#指定编译器
CC = g++

#编译选项 -fPIC -shared
CFLAGS =  -std=c++14 
$(warning CFLAGS is $(CFLAGS))

#头文件路径
INCLUDE := -I/home/timer/Fast-DDS/install/include   #FastDDS的头文件目录
INCLUDE += -I/home/timer/gtest/build/_install/include #gtest的头文件目录
INCLUDE += -I../../            #cmw目录的上一级目录
INCLUDE += -I../thirdparty/    #cmw 引入的第三方库的目录
# INCLUDE := -I/data/Fast-DDS/install/include   #FastDDS的头文件目录
# INCLUDE += -I/data/                  #cmw目录的上一级目录


#找出当前目录下，所有的源文件
SRCS := $(shell find ../* -type f | grep '\.cpp' | grep -v './test.*\.cpp')
$(warning SRCS is ${SRCS})

# 找出所有的汇编文件
ASMS := $(shell find ../* -type f | grep '\.S')
$(warning ASMS is ${ASMS})

#确定cpp源文件对应的目标文件
OBJS := $(patsubst %.cpp, %.o , $(filter %.cpp, $(SRCS)))

# 确定汇编源文件对应的目标文件
OBJS += $(patsubst %.S, %.o, $(filter %.S, $(ASMS)))
$(warning OBJS is ${OBJS})

# 编译目标文件的规则，确保包含头文件路径
%.o: %.cpp
	$(CC) $(INCLUDE) $(CFLAGS) -c -o $@ $<

# 编译汇编文件规则
%.o: %.S
	$(AS) -c $< -o $@

#第三方动态库的路径
ThirdPartyLibPath := -L/home/timer/Fast-DDS/install/lib #FastDDS的动态库目录
ThirdPartyLibPath += -L/home/timer/gtest/build/_install/lib #gtest的动态库目录
# ThirdPartyLibPath := /data/Fast-DDS/install/lib #FastDDS的动态库目录

#第三方动态库
ThirdPartyLib := -lfastrtps -lfastcdr -lfoonathan_memory-0.7.3 
ThirdPartyLib += -lpthread
ThirdPartyLib += -luuid
ThirdPartyLib += -lrt
ThirdPartyLib += -latomic
ThirdPartyLib += -lgtest 

# 动态库名称
SHARED_LIB = libcmw_v3.so

# 生成动态库的规则
# .PHONY: libcmw
# libcmw: $(SHARED_LIB)
# $(SHARED_LIB): $(OBJS)
# 	$(CC) $(INCLUDE) $(CFLAGS) -shared -o $@ $^ -L$(ThirdPartyLibPath) $(ThirdPartyLib)

test_writer: $(OBJS) test_writer.cpp
	$(CC) $(INCLUDE) -o $@.out $^  $(ThirdPartyLibPath) $(ThirdPartyLib) 

test_reader: $(OBJS) test_reader.cpp
	$(CC) $(INCLUDE) -o $@.out $^  $(ThirdPartyLibPath) $(ThirdPartyLib) 

test_serialize: $(OBJS) test_serialize.cpp
	$(CC) $(INCLUDE) -o $@.out $^  $(ThirdPartyLibPath) $(ThirdPartyLib) 

test_node_manager: $(OBJS) test_node_manager.cpp
	$(CC) $(INCLUDE) -o $@.out $^  $(ThirdPartyLibPath) $(ThirdPartyLib) 

test_channel_manager: $(OBJS) test_channel_manager.cpp
	$(CC) $(INCLUDE) -o $@.out $^  $(ThirdPartyLibPath) $(ThirdPartyLib) 

test_topology_manager: $(OBJS) test_topology_manager.cpp
	$(CC) $(INCLUDE) -o $@.out $^  $(ThirdPartyLibPath) $(ThirdPartyLib) 
	
test_publisher: $(OBJS) test_publisher.cpp 
	$(CC) $(INCLUDE) -o $@.out $^  $(ThirdPartyLibPath) $(ThirdPartyLib) 

test_subscriber: $(OBJS) test_subscriber.cpp
	$(CC) $(INCLUDE) -o $@.out $^  $(ThirdPartyLibPath) $(ThirdPartyLib) 

test_log: $(OBJS) test_log.cpp
	$(CC) $(INCLUDE) -o $@.out $^  $(ThirdPartyLibPath) $(ThirdPartyLib) 

test_getenv : test_getenv.cpp	
	$(CC) -o $@.out $^

test_croutine: $(OBJS) test_croutine.cpp
	$(CC) $(INCLUDE) -o $@.out $^  $(ThirdPartyLibPath) $(ThirdPartyLib) 

.PHONY:clean
clean:
	rm -rf *.out 
	rm -rf *.log
	rm -rf $(OBJS)