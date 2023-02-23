randmst: randmst.cpp
	g++ -std=c++11 -O3 -o randmst randmst.cpp

clean:
	rm -f randmst randmst.o