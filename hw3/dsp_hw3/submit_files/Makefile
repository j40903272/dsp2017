# The following two variable will be commandline determined by TA
# For testing, you could uncomment them.
SRIPATH ?= $(HOME)/srilm-1.5.10
MACHINE_TYPE ?= i686-m64
LM ?= bigram.lm

CXX = g++
CXXFLAGS = -g -O3 -std=c++11 -I$(SRIPATH)/include -w
vpath lib%.a $(SRIPATH)/lib/$(MACHINE_TYPE)

TARGET = mydisambig
SRC = mydisambig.cpp
OBJ = $(SRC:.cpp=.o)
MAP = ZhuYin-Big5.map
ORDER = 2
.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(OBJ) -loolm -ldstruct -lmisc
	$(CXX) $(LDFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $<
run:
	@python3 z2b.py
	@for i in $(shell seq 1 10) ; do \
		./mydisambig testdata/$$i.txt $(LM) $(ORDER) > result2/$$i.txt; \
	done;
clean:
	$(RM) $(OBJ) $(TARGET)

