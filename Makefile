CXX=g++
LINK=-lm -lX11
DEBUG=-g -DDEBUG -Og -ggdb
CXXFLAGS=$(DEBUG)
COMPILE=$(CXX) $(CXXFLAGS) 

BUILD=build
OBJECTS = $(BUILD)/Draw.o $(BUILD)/x11layer.o $(BUILD)/convexhull.o

MPICXX=mpicxx
MPI_LINK=$(shell $(MPICXX) --showme:link) /usr/lib64/valgrind/libmpiwrap-amd64-linux.so
MPI_COMPILE_FLAGS=$(shell $(MPICXX) --showme:compile)
MPI_CXXFLAGS=$(CXXFLAGS) $(MPI_COMPILE_FLAGS)
MPI_COMPILE=$(MPICXX) $(MPI_CXXFLAGS)

all: $(BUILD)/compgeo

$(BUILD)/compgeo: compgeo.cpp $(OBJECTS)
	$(MPI_COMPILE) -o $@ $^ $(LINK) $(MPI_LINK)

$(BUILD)/Draw.o: Draw.cpp Draw.h compgeo.h
$(BUILD)/x11layer.o: x11layer.c x11layer.h Draw.h
$(BUILD)/convexhull.o: convexhull.cpp compgeo.h
$(OBJECTS): | $(BUILD)
	$(COMPILE) -c $< -o $@

$(BUILD):
	mkdir -p $(BUILD)

clean:
	rm -rf $(BUILD)

.PHONY: all rebuild clean
