CC = g++
CFLAGS = -std=c++14

INCLUDE := -I/home/timer/gtest/build/_install/include \
		   -I../../../ \

all: libplugin1.so libplugin2.so

libplugin1.so: 
	g++ -shared -fPIC -o $@ plugin1.cc $(INCLUDE)

libplugin2.so: 
	g++ -shared -fPIC -o $@ plugin2.cc $(INCLUDE)

# 清理规则
.PHONY: clean
clean:
	rm -rf *.so
