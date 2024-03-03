BIN=bin

CODEDIRS=. ../lib
INCLUDES=-I. -I../lib
LIBS=

CC=g++
OPT=-O0

DEP_FLAGS=-MP -MD
CFLAGS=-Wall -Wextra -g $(INCLUDES) $(OPT) $(DEP_FLAGS)

CPPFILES=$(foreach DIR,$(CODEDIRS),$(wildcard $(DIR)/*.cpp))
OFILES=$(patsubst %.cpp,%.o,$(CPPFILES))
DFILES=$(patsubst %.cpp,%.d,$(CPPFILES))

all:$(BIN)

$(BIN):$(OFILES)
	$(CC) -o $@ $^ $(LIBS)

%.o:%.cpp
	$(CC) $(CFLAGS) -c -o $@ $< $(LIBS)

clean:
	rm -rf $(BIN) $(OFILES) $(DFILES)

-include $(DFILES)
