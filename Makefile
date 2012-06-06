# objektitiedostot
SRC = main.cc camera.cc matrix.cc objreader.cc debug.cc texture.cc \
solidmaterial.cc shader.cc
OBJS = $(SRC:.cc=.o)

CXX = LC_ALL=C g++
CXXDEP = g++
CXXFLAGS = -Wall -pedantic
LIBS = -lGL -lglfw -lm -lassimp -lGLEW
EXE = ohjelma

default: $(EXE)

%.d: %.cc
	$(CXXDEP) $(CXXFLAGS) -MM -MQ $(<:%.cc=%.o) -MQ $(<:%.cc=%.d) -o $@ $<

ifneq ($(MAKECMDTARGETS),clean)
-include $(OBJS:.o=.d)
endif

$(EXE): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LIBS)

clean:
	-rm $(OBJS) $(OBJS:.o=.d) $(EXE) $(DEPFILE)

trace:
	apitrace trace --output=ohjelma.trace ./ohjelma
	qapitrace ohjelma.trace

.PHONY: clean debug
