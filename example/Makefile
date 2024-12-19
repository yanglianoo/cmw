# 编译器和选项
CC = g++
CFLAGS = -std=c++14
INCLUDE := -I/home/timer/Fast-DDS/install/include \
           -I/home/timer/gtest/build/_install/include \
           -I../../ \
           -I../thirdparty/

ThirdPartyLibPath := -L/home/timer/Fast-DDS/install/lib \
                     -L/home/timer/gtest/build/_install/lib

ThirdPartyLib := -lfastrtps -lfastcdr -lfoonathan_memory-0.7.3 \
                 -lpthread -luuid -lrt -latomic -lgtest -ldl

# 可执行文件列表
EXECUTABLES := test_writer test_reader test_serialize test_node_manager \
              test_channel_manager test_topology_manager test_publisher \
              test_subscriber test_log test_getenv test_croutine test_scheduler \
              test_task test_blocker test_class_loader \


# 找出源文件
SRCS := $(shell find ../* -type f | grep '\.cpp' | grep -v './test.*\.cpp')
ASMS := $(shell find ../* -type f | grep '\.S')
OBJS := $(patsubst %.cpp, %.o, $(SRCS)) $(patsubst %.S, %.o, $(ASMS))

# 编译规则
%.o: %.cpp
	$(CC) $(INCLUDE) $(CFLAGS) -c -o $@ $<

%.o: %.S
	$(AS) -c $< -o $@

# 生成可执行文件的规则
$(EXECUTABLES): %: $(OBJS) %.cpp
	$(CC) $(INCLUDE) -o $@.out $^ $(ThirdPartyLibPath) $(ThirdPartyLib)

# 清理规则
.PHONY: clean
clean:
	rm -rf *.out *.log $(OBJS)
