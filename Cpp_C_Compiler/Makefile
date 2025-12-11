CC = g++
CFLAGS = -W -Wall -pedantic -std=c++17
YFLAGS = -d -v # --language=c++
CLIBS = -ll
YACC = bison
LEX = flex

OUT = a.out

SRC_DIR     = src
INC_DIR     = include
BUILD_DIR   = build

INCLUDES 	= -I./$(INC_DIR)

CFLAGS 		+= $(INCLUDES)

LEX_SRC     = lexer.ll
YACC_SRC    = parser.yy

LEX_GEN_CC  = $(SRC_DIR)/lex.yy.cc
LEX_GEN_O   = $(SRC_DIR)/lex.yy.o

YACC_GEN_CC = $(SRC_DIR)/parser.tab.cc
YACC_GEN_O  = $(SRC_DIR)/parser.tab.o
YACC_GEN_HH = $(INC_DIR)/parser.tab.hh

SRC         = $(wildcard $(SRC_DIR)/*.cc)
OBJ         = $(SRC:.cc=.o)


ifdef DEBUG
	CFLAGS += -g3 -DDEBUG
	YFLAGS += --debug -t -Wcounterexamples
endif


all : $(OBJ) $(YACC_GEN_HH) $(YACC_GEN_O) $(LEX_GEN_O) 
	$(CC) $^ -o $(OUT) $(CLIBS)

$(YACC_GEN_HH) $(YACC_GEN_CC) parser.output: $(YACC_SRC)
	$(YACC) --defines=$(YACC_GEN_HH) --output=$(YACC_GEN_CC) $(YFLAGS) $<

$(YACC_GEN_O) : $(YACC_GEN_CC) $(YACC_GEN_HH)
	$(CC) -c $< $(CFLAGS) -o $@

$(LEX_GEN_O): $(LEX_GEN_CC) $(YACC_GEN_HH)
	$(CC) -c $< $(CFLAGS) -o $@

$(LEX_GEN_CC) : $(LEX_SRC)
	$(LEX) -o $@ $< 

%.o : %.cc
	$(CC) -c $< $(CFLAGS) -o $@

clean:
	rm -rf $(YACC_GEN_HH) src/*.yy.* src/*.tab.* src/*.o $(OUT)


.PHONY: all clean