CXX=g++ -std=c++1z -march=native -mtune=native
CXXFLAGS=-Wall -pedantic -Ofast

.PHONY: speedcheck

speedcheck: tcps
	./tcps

tcps: test_thread.cpp do_something.cpp
	$(CXX) $(CXXFLAGS) -pthread -o "$@" $^
