CPPC = g++
CPPFLAGS = -Wall -Wextra -O2 -std=c++17 -O3
LDFLAGS = 

all: phase1 phase2 phase3 api

phase1: phase1.o distances.o light.o file_operations.o data_structs.h
	$(CPPC) $(CPPFLAGS) -o $@ $^ $(LDFLAGS)

phase2: phase2.o distances.o light.o data_structs.h
	$(CPPC) $(CPPFLAGS) -o $@ $^ $(LDFLAGS)

phase3: phase3.o distances.o light.o data_structs.h
	$(CPPC) $(CPPFLAGS) -o $@ $^ $(LDFLAGS)

api: api.o distances.o light.o data_structs.h
	$(CPPC) $(CPPFLAGS) -o $@ $^ $(LDFLAGS)

phase1.o: phase1.cpp data_structs.h
	$(CPPC) $(CPPFLAGS) -c -o $@ $<

phase2.o: phase2.cpp data_structs.h
	$(CPPC) $(CPPFLAGS) -c -o $@ $<

phase3.o: phase3.cpp data_structs.h
	$(CPPC) $(CPPFLAGS) -c -o $@ $<

api.o: api.cpp data_structs.h
	$(CPPC) $(CPPFLAGS) -c -o $@ $<

distances.o: distances.cpp distances.h data_structs.h
	$(CPPC) $(CPPFLAGS) -c -o $@ $<

file_operations.o: file_operations.cpp file_operations.h data_structs.h
	$(CPPC) $(CPPFLAGS) -c -o $@ $<

light.o: light.cpp light.h data_structs.h
	$(CPPC) $(CPPFLAGS) -c -o $@ $<


clean:
	rm -f *.o phase1 phase2 phase3 api