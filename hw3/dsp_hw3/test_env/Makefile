# The following two variable will be commandline determined by TA
# For testing, you could uncomment them.
#SRIPATH ?= /data/DSP_HW3/103_2/srilm-1.5.10
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
	@#TODO How to run your code toward different txt? 
	@for i in $(shell seq 1 10) ; do \
		echo "Running $$i.txt"; \
		./mydisambig testdata/$$i.txt $(LM) $(ORDER) > result2/$$i.txt; \
	done;
clean:
	$(RM) $(OBJ) $(TARGET)

