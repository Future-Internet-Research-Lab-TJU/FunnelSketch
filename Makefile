GCC = g++
CFLAGS = -O2 -std=c++14 
SSEFLAGS = -msse2 -msse3 -msse4.1 -msse4.2 -mavx -march=native
FILES = frequency throughput heavyhitter heavychanger


all: $(FILES) 

frequency: frequency.cpp
	$(GCC) $(CFLAGS) frequency.cpp -o frequency.out

throughput: throughput.cpp
	$(GCC) $(CFLAGS) throughput.cpp -o throughput.out

heavyhitter: heavyhitter.cpp
	$(GCC) $(CFLAGS) heavyhitter.cpp -o heavyhitter.out

heavychanger: heavychanger.cpp
	$(GCC) $(CFLAGS) heavychanger.cpp -o heavychanger.out

clean:
	rm $(all) -f *~ *.o *.out
