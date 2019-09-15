cleaner:
	g++ -c -Wall -std=c++11 *.cpp
	g++ -o cleaner *.o sqdb/sqdb.o sqdb/sqlite3.o #call make in sqdb?
clean:
	rm cleaner
