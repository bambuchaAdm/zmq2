

.PHONY = clean

all:
	g++ zmq2.cpp -c

clean:
	rm -rf *.o