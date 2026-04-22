CXX=g++
CXXFLAGS?= -Wall -O0
OUTFILES=assembler.*
INFILES=assembler/assembler.c assembler/encoders.c assembler/flags.c

UNAME := $(shell uname)

ifeq ($(UNAME), Linux)
	OUTPUT := fga.o	
endif
ifeq ($(UNAME), Darwin)
	OUTPUT := fga.o	
endif
ifeq ($(OS), Windows_NT)
	OUTPUT := fga.exe
endif

# all: $(CXX) $(CXXFLAGS) assembler.c -o assembler.o
all: $(INFILES)
	$(CXX) $(CXXFLAGS) $(INFILES) -o $(OUTPUT)

clean:
	$(RM) $(OUTFILES) *.exe *.o