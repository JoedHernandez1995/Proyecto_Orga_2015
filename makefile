main:	main.o primaryKey.o 
	g++ main.o primaryKey.o -o main

main.o:	main.cpp primaryKey.h
	g++ -c main.cpp

primaryKey.o:	primaryKey.cpp primaryKey.h
	g++ -c primaryKey.cpp

clean:
	rm *.o main
