main:	main.o primaryKey.o nodo.o arbolb.o
	g++ main.o primaryKey.o nodo.o arbolb.o -o main

main.o:	main.cpp primaryKey.h nodo.h arbolb.h
	g++ -c main.cpp

primaryKey.o:	primaryKey.cpp primaryKey.h
	g++ -c primaryKey.cpp

nodo.o:	nodo.cpp nodo.h 
	g++ -c nodo.cpp

arbolb.o:	arbolb.cpp arbolb.h 
	g++ -c arbolb.cpp

clean:
	rm *.o main
