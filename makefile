CFLAGS   = -Wno-endif-labels -ggdb
LDFLAGS  =
PROG = realms_shattered
CXX = g++

DIRS =  src/engine_systems/*.cpp  src/entities/*.cpp   src/game_state/*.cpp   src/platform/*.cpp src/objects/*.cpp 
# top-level rule to create the program.
all: $(PROG)

$(PROG): src/*
	$(CXX) $(CFLAGS) src/main.cpp $(DIRS) -o $(PROG) 

# cleaning everything that can be automatically recreated with "make"
clean:
	rm $(PROG)
