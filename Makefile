cleaner:
	g++ -c -Wall -std=c++11 *.cpp
	g++ -o cleaner.out *.o sqdb/sqdb.o sqdb/sqlite3.o
clean:
	rm cleaner.out
