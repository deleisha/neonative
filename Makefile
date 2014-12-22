webserver: main.cc
	$(MAKE) -C deps/libuv
	g++ --std=c++0x -g -o webserver main.cc -I deps/libuv/include deps/libuv/.libs/libuv.a -lpthread


clean:
	-rm webserver
