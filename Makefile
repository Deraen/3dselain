# objektitiedostot
SRC = $(addprefix ../src/, \
camera.cc \
scene.cc \
debug.cc \
texture.cc \
shader.cc \
manager.cc \
common.cc \
block.cc \
lumploader.cc \
gl3w.c \
)

OBJS = $(addprefix build/, \
$(subst ../src/,, $(patsubst %.cc, %.o, $(filter %.cc, $(SRC)))) \
$(subst ../src/,, $(patsubst %.c, %.o, $(filter %.c, $(SRC)))) \
)

DEPFILES = $(OBJS:%.o=%.d)

CXX = LC_ALL=C g++-4.7
CXXDEP = g++-4.7
CXXFLAGS = -std=c++11 -Wall -pedantic -Iinclude/
CFLAGS = -Wall -Iinclude/
LIBS = -lGL -lassimp -lglfw -lboost_system -lboost_filesystem -lboost_iostreams
DBGFLAGS= -g

EXE = ohjelma
EXE2 = converter
DEBUG-EXE = debug
DEBUG-OBJS = $(OBJS:%.o=%.dbg.o)

default: $(EXE) $(EXE2)

ifneq ($(MAKECMDTARGETS),clean)
-include $(DEPFILES)
-include build/main.d
-include build/converter.d
endif

gl3w:
	./gl3w/gl3w_gen.py

%.d: ../src/%.cc
	$(CXXDEP) $(CXXFLAGS) -MM -MQ $(subst ../src/,, $(<:%.cc=%.o)) -MQ $(subst ../src/,, $(<:%.cc=%.d)) -o $@ $<
%.o: ../src/%.cc
	$(CXX) $(CXXFLAGS) -c -o $@ $<
%.dbg.o: ../src/%.cc
	$(CXX) $(CXXFLAGS) $(DBGFLAGS) -c -o $@ $<

%.d: ../src/%.c
	$(CC) $(CFLAGS) -MM -MQ $(subst ../src/,, $(<:%.c=%.o)) -MQ $(subst ../src/,, $(<:%.c=%.d)) -o $@ $<
%.o: ../src/%.c
	$(CC) $(CFLAGS) -c -o $@ $<
%.dbg.o: ../src/%.c
	$(CC) $(CFLAGS) $(DBGFLAGS) -c -o $@ $<

$(EXE): $(OBJS) build/main.o
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LIBS)

$(EXE2): $(OBJS) build/converter.o
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LIBS)

clang:
	make CXX=clang++ $(EXE)

$(DEBUG-EXE): $(DEBUG-OBJS)
	$(CXX) $(CXXFLAGS) $(DBGFLAGS) -o $@ $^ $(LIBS)

clean:
	-rm $(EXE) $(EXE2) $(DEBUG-EXE) build/*

trace: $(EXE)
	apitrace trace --output=ohjelma.trace ./ohjelma
	qapitrace ohjelma.trace

.PHONY: clean debug gl3w
