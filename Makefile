CXX=g++
LINK=-lm -lX11
DEBUG=-g -DDEBUG -Og -ggdb
CXXFLAGS=$(DEBUG)
COMPILE=$(CXX) $(CXXFLAGS) 

MPICXX=mpicxx
MPI_LINK=$(LINK) $(shell $(MPICXX) --showme:link) /usr/lib64/valgrind/libmpiwrap-amd64-linux.so
MPI_COMPILE_FLAGS=$(shell $(MPICXX) --showme:compile)
MPI_CXXFLAGS=$(CXXFLAGS) $(MPI_COMPILE_FLAGS)
MPI_COMPILE=$(MPICXX) $(MPI_CXXFLAGS)
BUILD=build

all: compgeo

compgeo: compgeo.cpp $(BUILD)/Draw.o $(BUILD)/x11layer.o $(BUILD)/convexhull.o
	$(MPI_COMPILE) -o $(BUILD)/compgeo compgeo.cpp $(BUILD)/*.o $(MPI_LINK)

$(BUILD)/Draw.o: Draw.cpp Draw.h compgeo.h build
	$(COMPILE) -c -o $(BUILD)/Draw.o Draw.cpp

$(BUILD)/x11layer.o: x11layer.c x11layer.h Draw.h build
	$(COMPILE) -c -o $(BUILD)/x11layer.o x11layer.c

$(BUILD)/convexhull.o: convexhull.cpp compgeo.h build
	$(COMPILE) -c -o $(BUILD)/convexhull.o convexhull.cpp

build:
	mkdir -p $(BUILD)

clean:
	rm -f \#*\#
	rm -f *~
	rm -rf $(BUILD) compgeo

.PHONY: build clean
