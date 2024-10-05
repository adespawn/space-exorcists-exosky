CPPC = g++
CPPFLAGS = -Wall -Wextra -O2 -std=c++17 -O3
LDFLAGS = 

all: main

main: main.o distances.o light.o file_operations.o data_structs.h
	$(CPPC) $(CPPFLAGS) -o $@ $^ $(LDFLAGS)

main.o: main.cpp data_structs.h
	$(CPPC) $(CPPFLAGS) -c -o $@ $<

distances.o: distances.cpp distances.h data_structs.h
	$(CPPC) $(CPPFLAGS) -c -o $@ $<

file_operations.o: file_operations.cpp file_operations.h data_structs.h
	$(CPPC) $(CPPFLAGS) -c -o $@ $<

light.o: light.cpp light.h data_structs.h
	$(CPPC) $(CPPFLAGS) -c -o $@ $<


clean:
	rm -f *.o main