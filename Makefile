#
#	Makefile for this SNAP example
#	- modify Makefile.ex when creating a new SNAP example
#
#	implements:
#		all (default), clean
#

#
#	Global configuration for SNAP makefiles
#

GLIB = glib-core
SNAP = snap-core
GLIBADV = glib-adv
SNAPADV = snap-adv
SNAPEXP = snap-exp

CGLIB = ../$(GLIB)
CSNAP = ../$(SNAP)

EXGLIB = ../../$(GLIB)
EXSNAP = ../../$(SNAP)
EXGLIBADV = ../../$(GLIBADV)
EXSNAPADV = ../../$(SNAPADV)
EXSNAPEXP = ../../$(SNAPEXP)

UNAME := $(shell uname)

## Main application file
MAIN = ctip
GEN = generate_nets
DEPH =  $(EXSNAPADV)/kronecker.h
DEPCPP = $(EXSNAPADV)/kronecker.cpp cascdynetinf.cpp



ifeq ($(UNAME), Linux)
  # Linux flags
  CC = g++
  CXXFLAGS += -std=c++11 -Wall 
  CXXFLAGS += -O3 -DNDEBUG
  # turn on for crash debugging, get symbols with <prog> 2>&1 | c++filt
  #CXXFLAGS += -g -rdynamic
  CXXOPENMP = -fopenmp
  LDFLAGS +=
  LIBS += -lrt

else ifeq ($(UNAME), Darwin)
  # OS X flags
  CC = g++
  CXXFLAGS += -std=c++11 -Wall
  CXXFLAGS += -O3 -DNDEBUG
  CXXOPENMP = -fopenmp
  CLANG := $(shell g++ -v 2>&1 | grep clang | cut -d " " -f 2)
  ifeq ($(CLANG), LLVM)
    CXXOPENMP =
    CXXFLAGS += -DNOMP
  endif

  LDFLAGS +=
  LIBS +=

else ifeq ($(shell uname -o), Cygwin)
  # Cygwin flags
  CC = gcc
  CXXFLAGS += -Wall -D__STDC_LIMIT_MACROS
  CXXFLAGS += -O3 -DNDEBUG
  CXXOPENMP = -fopenmp
  LDFLAGS +=
  LIBS +=

endif





#
# Makefile for non-Microsoft compilers
#

all: $(MAIN) $(GEN)

# COMPILE
$(MAIN): $(MAIN).cpp $(DEPH) $(DEPCPP) $(EXSNAP)/Snap.o 
	$(CC) $(CXXFLAGS) -o $(MAIN) $(MAIN).cpp $(DEPCPP) $(EXSNAP)/Snap.o -I$(EXSNAP) -I$(EXSNAPADV) -I$(EXGLIB) -I$(EXSNAPEXP) $(LDFLAGS) $(LIBS)




$(GEN): $(GEN).cpp $(DEPH) $(DEPCPP) $(EXSNAP)/Snap.o 
	$(CC) $(CXXFLAGS) -o $(GEN) $(GEN).cpp $(DEPCPP) $(EXSNAP)/Snap.o -I$(EXSNAP) -I$(EXSNAPADV) -I$(EXGLIB) -I$(EXSNAPEXP) $(LDFLAGS) $(LIBS)




$(EXSNAP)/Snap.o: 
	make -C $(EXSNAP)

clean:
	rm -f *.o  $(MAIN)  $(MAIN).exe
	rm -rf Debug Release


#
#	configuration variables for the example


