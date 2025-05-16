CXX=g++
CXXFLAGS?= -Wall -O0
OUTFILES=assembler.*
INFILES=assembler/assembler.c assembler/encoders.c assembler/flags.c

# all: $(CXX) $(CXXFLAGS) assembler.c -o assembler.o
all: $(OUTFILES)

windows: assembler/assembler.c
	$(CXX) $(CXXFLAGS) $(INFILES) -o fga.exe
# $(CXX) $(CXXFLAGS) assembler.c
linux: assembler/assembler.c
	$(CXX) $(CXXFLAGS) assembler.c -o fga.o

windows_d: assembler/assembler.c
	$(CXX) $(CXXFLAGS) $(INFILES) -g -o fga.exe

clean:
	$(RM) $(OUTFILES) *.exe *.o