INCLUDE:= -I/home/timer/Fast-DDS/install/include
INCLUDE+= -I/home/timer/Fast-DDS/cmw
rtpsLibPath:= /home/timer/Fast-DDS/install/lib

test_rtps:*.cpp ../serialize/data_stream.cpp
	g++  $(INCLUDE) -o $@ $^ -L$(rtpsLibPath) -lfastrtps -lfastcdr -lfoonathan_memory-0.7.3 -Wl,-rpath=$(rtpsLibPath)