INCLUDE += -I/home/timer/Fast-DDS
#指定编译器
CC = g++

.PHONY:test_serialize
test_serialize:test_serialize.cpp ../serialize/data_stream.cpp
	$(CC) $^ $(INCLUDE) -o $@