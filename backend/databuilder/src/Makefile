CPPC = g++
CPPFLAGS = -Wall -Wextra -O2 -std=c++17 -O3
LDFLAGS = 

all: main

main: main.o distances.o light.o file_operations.o
	$(CPPC) $(CPPFLAGS) -o $@ $^ $(LDFLAGS)

main.o: main.cpp 
	$(CPPC) $(CPPFLAGS) -c -o $@ $<

distances.o: distances.cpp distances.h
	$(CPPC) $(CPPFLAGS) -c -o $@ $<

file_operations.o: file_operations.cpp file_operations.h
	$(CPPC) $(CPPFLAGS) -c -o $@ $<

light.o: light.cpp light.h
	$(CPPC) $(CPPFLAGS) -c -o $@ $<


clean:
	rm -f *.o kierki-serwer kierki-klient