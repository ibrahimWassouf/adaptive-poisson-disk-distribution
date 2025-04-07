adaptive : adaptive_poisson.cpp rec.cpp kd-tree.cpp index_pq.cpp
	g++ -std=c++20 rec.cpp kd-tree.cpp adaptive_poisson.cpp index_pq.cpp -O3 -o adaptive.o

random : random.cpp rec.cpp kd-tree.cpp index_pq.cpp
	 g++ -std=c++20 rec.cpp kd-tree.cpp index_pq.cpp random.cpp -o random.o -g

test : rec.cpp kd-tree.cpp
	g++ -std=c++20 tests.cpp rec.cpp kd-tree.cpp -o tests.o -g


