CC = clang++ -std=c++11

all:
	$(CC) main.cpp timespiece.cpp -o timespiece

test:

clean:
	rm -r ./timespiece
