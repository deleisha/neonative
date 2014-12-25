webserver: main.cc
	$(MAKE) -C deps/libuv
	g++ --std=c++0x -g -Wall -o webserver main.cc -I deps/libuv/include deps/libuv/.libs/libuv.a -lpthread -lm -lrt


clean:
	-rm webserver
