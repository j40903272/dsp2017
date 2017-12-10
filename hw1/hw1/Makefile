.PHONY: all clean

CFLAGS += -g -pg -fopenmp -O2
CXXFLAGS += -g -pg -fopenmp -O2
LDFLAGS += -lm     # link to math library

TARGET=test train

all: $(TARGET)
# type make/make all to compile test_hmm

clean:
	$(RM) $(TARGET)   # type make clean to remove the compiled file
