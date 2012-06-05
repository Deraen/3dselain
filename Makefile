# objektitiedostot
SRC = main.cc light.cc camera.cc matrix.cc objreader.cc debug.cc texture.cc solidmaterial.cc
OBJS = $(SRC:.cc=.o)

CXX = LC_ALL=C g++
CXXDEP = g++
CXXFLAGS = -Wall -pedantic -Werror
LIBS = -lGL -lglfw -lm -lassimp
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
