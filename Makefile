test_demo: test_demo.o MemMgr.o
	g++ -o test_demo test_demo.o MemMgr.o -lpthread

test_demo.o: test_demo.cpp src/MemMgr.h
	g++ -c test_demo.cpp

MemMgr.o: src/MemMgr.cpp src/MemMgr.h
	g++ -c src/MemMgr.cpp

clean:
	rm -f test_demo	*.o

