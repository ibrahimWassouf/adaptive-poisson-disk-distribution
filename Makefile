adaptive : adaptive_poisson.cpp rec.cpp kd-tree.cpp
	g++ -std=c++20 rec.cpp kd-tree.cpp adaptive_poisson.cpp -o adaptive.o -g

regular : poisson.cpp rec.cpp kd-tree.cpp
	 g++ -std=c++20 rec.cpp kd-tree.cpp poisson.cpp -o poisson.o -g

adaptive_poisson : rec kd-tree
	g++ -std=c++20 adaptive_poisson.cpp rec.o kd-tree.o -g

kd-tree : rec
	g++ -std=c++20 kd-tree.cpp rec.o -g

rec :
	g++ -std=c++20 rec.cpp -g

