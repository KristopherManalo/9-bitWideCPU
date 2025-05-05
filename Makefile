CXX=g++
CXXFLAGS?= -Wall -O0
OUTFILES=assembler.*
INFILES=assembler/assembler.c assembler/encoders.c

# all: $(CXX) $(CXXFLAGS) assembler.c -o assembler.o
all: $(OUTFILES)

windows: assembler/assembler.c
	$(CXX) $(CXXFLAGS) $(INFILES) -o assembler.exe
# $(CXX) $(CXXFLAGS) assembler.c
linux: assembler/assembler.c
	$(CXX) $(CXXFLAGS) assembler.c -o assembler.o

clean:
	$(RM) $(OUTFILES) *.exe *.o