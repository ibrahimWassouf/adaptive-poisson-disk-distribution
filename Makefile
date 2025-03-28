adaptive : adaptive_poisson.cpp rec.cpp kd-tree.cpp
	g++ -std=c++20 rec.cpp kd-tree.cpp adaptive_poisson.cpp index_pq.cpp -o adaptive.o -g -lprofiler

regular : poisson.cpp rec.cpp kd-tree.cpp
	 g++ -std=c++20 rec.cpp kd-tree.cpp poisson.cpp -o poisson.o -g

adaptive_poisson : rec kd-tree
	g++ -std=c++20 adaptive_poisson.cpp rec.o kd-tree.o -g

test : rec.cpp kd-tree.cpp
	g++ -std=c++20 tests.cpp rec.cpp kd-tree.cpp -o tests.o -g
kd-tree : rec
	g++ -std=c++20 kd-tree.cpp rec.o -g

rec :
	g++ -std=c++20 rec.cpp -g

