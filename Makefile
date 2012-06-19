# objektitiedostot
SRC = main.cc camera.cc matrix.cc objreader.cc debug.cc texture.cc \
solidmaterial.cc shader.cc manager.cc
OBJS = $(SRC:.cc=.o) src/gl3w.o

CXX = LC_ALL=C g++
CXXDEP = g++
CXXFLAGS = -Wall -pedantic -Iinclude/
CFLAGS = -Wall -Iinclude/
LIBS = -lGL -lglfw -lm -lassimp
DBGFLAGS= -g
EXE = ohjelma

default: $(EXE)

gl3w:
	./gl3w/gl3w_gen.py

%.d: %.cc
	$(CXXDEP) $(CXXFLAGS) -MM -MQ $(<:%.cc=%.o) -MQ $(<:%.cc=%.d) -o $@ $<

%.dbg.o: %.cc
	$(CXX) $(CXXFLAGS) $(DBGFLAGS) -c -o $@ $^
%.dbg.o: %.c
	$(CC) $(CFLAGS) $(DBGFLAGS) -c -o $@ $^

ifneq ($(MAKECMDTARGETS),clean)
-include $(OBJS:.o=.d)
endif

$(EXE): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LIBS)

debug: $(OBJS:%.o=%.dbg.o)
	$(CXX) $(CXXFLAGS) $(DBGFLAGS) -o $@ $^ $(LIBS)

clean:
	-rm $(OBJS) $(OBJS:.o=.d) $(EXE) $(DEPFILE)

trace: $(EXE)
	apitrace trace --output=ohjelma.trace ./ohjelma
	qapitrace ohjelma.trace

.PHONY: clean debug gl3w
