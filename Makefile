all: rubic


CILK = 
CXXFLAGS = -g -std=c++11 $(CILK)

rubic: rubics.cpp
	g++ $(CXXFLAGS) rubics.cpp -o rubics_exe

clean:
	rm -rf rubics_exe*
