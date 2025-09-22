adaptive : adaptive_poisson.cpp rec kd-tree index_pq
	g++ -std=c++20 rec.o kd-tree.o adaptive_poisson.cpp index_pq.o -O3 -o adaptive.o

rec : rec.cpp
	g++ -std=c++20 -c rec.cpp -o rec.o -g

kd-tree : kd-tree.cpp
	g++ -std=c++20 -c kd-tree.cpp -o kd-tree.o -g

index_pq : index_pq.cpp
	g++ -std=c++20 -c index_pq.cpp -o index_pq.o -g

random : random.cpp rec.cpp kd-tree.cpp index_pq.cpp
	 g++ -std=c++20 rec.cpp kd-tree.cpp index_pq.cpp random.cpp -o random.o -g

test : rec.cpp kd-tree.cpp
	g++ -std=c++20 tests.cpp rec.cpp kd-tree.cpp -o tests.o -g


