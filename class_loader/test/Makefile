CC = g++
CFLAGS = -std=c++14

INCLUDE := -I/home/timer/gtest/build/_install/include \
		   -I../../../ \

deps:= ../../log/logger.cpp \
	   ../class_loader.cpp \
	   ../utility/class_factory.cpp \
	   ../shared_library/shared_library.cpp \

all: libplugin1.so libplugin2.so

libplugin1.so: 
	g++ -shared -fPIC -o $@ $(deps) plugin1.cc $(INCLUDE)

libplugin2.so: 
	g++ -shared -fPIC -o $@ $(deps) plugin2.cc $(INCLUDE)

# 清理规则
.PHONY: clean
clean:
	rm -rf *.so
