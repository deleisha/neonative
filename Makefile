webserver: main.cc
	$(MAKE) -C deps/libuv
	g++ --std=c++0x -g -Wall -o webserver main.cc -I deps/libuv/include deps/libuv/.libs/libuv.a -lpthread -lm -lrt


clean:
	-rm webserver
	-rm handle
	-rm tcp
	-rm stream


handle: handle.cc
	$(MAKE) -C deps/libuv
	g++ --std=c++0x -g -Wall -o handle handle.cc -I deps/libuv/include deps/libuv/.libs/libuv.a -lpthread -lm -lrt

stream: stream.cc
	$(MAKE) -C deps/libuv
	g++ --std=c++0x -g -Wall -o stream stream.cc -I deps/libuv/include deps/libuv/.libs/libuv.a -lpthread -lm -lrt

tcp: tcp.cc
	$(MAKE) -C deps/libuv
	g++ --std=c++0x -g -Wall -o tcp tcp.cc -I deps/libuv/include deps/libuv/.libs/libuv.a -lpthread -lm -lrt


test: handle stream tcp
