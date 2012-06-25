# objektitiedostot
SRC = $(addprefix ../src/, \
main.cc \
camera.cc \
scene.cc \
debug.cc \
texture.cc \
solidmaterial.cc \
shader.cc \
manager.cc \
common.cc \
block.cc \
lumploader.cc \
)

C_SRC = $(addprefix ../src/, \
gl3w.c \
)

OBJS = $(addprefix build/, $(subst ../src/,, $(SRC:.cc=.o)) $(subst ../src/,, $(C_SRC:.c=.o)))
DEPFILES = $(OBJS:%.o=%.d)

CXX = LC_ALL=C g++-4.7
CXXDEP = g++-4.7
CXXFLAGS = -std=c++11 -Wall -pedantic -Iinclude/
CFLAGS = -Wall -Iinclude/
LIBS = -lGL -lm -lassimp -lglfw
DBGFLAGS= -g

EXE = ../ohjelma
DEBUG-EXE = debug
DEBUG-OBJS = $(OBJS:%.o=%.dbg.o)

default: $(EXE)

ifneq ($(MAKECMDTARGETS),clean)
include $(DEPFILES)
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

$(EXE): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LIBS)

clang:
	make CXX=clang++ $(EXE)

$(DEBUG-EXE): $(DEBUG-OBJS)
	$(CXX) $(CXXFLAGS) $(DBGFLAGS) -o $@ $^ $(LIBS)

clean:
	-rm $(OBJS) $(EXE) $(DEBUG-EXE) $(DEBUG-OBJS) $(DEPFILES)

trace: $(EXE)
	apitrace trace --output=ohjelma.trace ./ohjelma
	qapitrace ohjelma.trace

.PHONY: clean debug gl3w
