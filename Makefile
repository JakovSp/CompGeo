C=g++
CFLAGS=-g
COMPILE=$(C) $(CFLAGS)
BUILD=build

all: compgeo

compgeo: compgeo.cpp $(BUILD)/Draw.o $(BUILD)/x11layer.o $(BUILD)/convexhull.o
	$(COMPILE) -o $(BUILD)/compgeo compgeo.cpp $(BUILD)/*.o -lm -lX11

$(BUILD)/Draw.o: Draw.cpp Draw.h compgeo.h build
	$(COMPILE) -c -o $(BUILD)/Draw.o Draw.cpp

$(BUILD)/x11layer.o: x11layer.c x11layer.h Draw.h build
	$(COMPILE) -c -o $(BUILD)/x11layer.o x11layer.c

$(BUILD)/convexhull.o: convexhull.cpp compgeo.h build
	$(COMPILE) -c -o $(BUILD)/convexhull.o convexhull.cpp

build:
	mkdir -p $(BUILD)

clean:
	rm -rf $(BUILD) compgeo

.PHONY: build clean
