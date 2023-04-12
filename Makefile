.PHONY: all clean

FLAGS = -Wall -Wextra -Werror -MMD -I include -std=c++17

OBJECTS = obj/main.o obj/huffman.o

EXE = huffman_compressor

all: $(EXE)

obj:
	mkdir -p obj

$(OBJECTS): obj/%.o: src/%.cpp
	g++ -c $(FLAGS) $< -o $@

$(EXE): obj $(OBJECTS)
	g++ $(FLAGS) $(OBJECTS) -o $(EXE) 

include $(wildcard obj/*.d)

clean:
	$(RM) $(EXE) 
	rm -f obj/*
	rm -fd obj
