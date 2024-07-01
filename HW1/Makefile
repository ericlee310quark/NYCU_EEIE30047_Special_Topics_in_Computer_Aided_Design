CC = g++

hw1.o : main.cpp QM_solver.o Petrick_solver.o
	$(CC) -std=c++11 -o 0812019.o 0812019.cpp QM_solver.o Petrick_solver.o -O2
QM_solver.o : QM_solver.h QM_solver.cpp
	$(CC) -std=c++11 -c QM_solver.cpp -O2
Petrick_solver.o: Petrick_solver.h Petrick_solver.cpp QM_solver.o
	$(CC) -std=c++11 -c Petrick_solver.cpp QM_solver.o -O2
clean :
	rm *.o
