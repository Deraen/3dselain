# objektitiedostot
SRC = main.cc engine.cc alusta.cc light.cc \
 solidmaterial.cc cube.cc sun.cc camera.cc \
 matrix.cc
OBJS = $(SRC:.cc=.o)

CXX = LC_ALL=C g++
CXXDEP = g++
CXXFLAGS = -Wall -pedantic -Werror
LIBS = -lglut -lGL -lGLU -lm
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

.PHONY: clean debug
