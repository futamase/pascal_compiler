CC = g++
OPTS = -O2 -Wall -std=c++14
OBJS = main.o compiler.o expression.o newfile.o output.o utility.o translation.o# sym_table.o

LIBDIR = $(HOME)/p1/lib
INCDIR = $(HOME)/p1/include

target: compiler

compiler: $(OBJS)
	$(CC) $(OBJS) -o compiler $(OPTS) -L$(LIBDIR) -lics
main.o: main.cpp
	$(CC) main.cpp -I $(INCDIR) -c $(OPTS)
newfile.o: newfile.cpp newfile.h
	$(CC) newfile.cpp -I $(INCDIR) -c $(OPTS)
utility.o: utility.cpp utility.h
	$(CC) utility.cpp -I $(INCDIR) -c $(OPTS)
compiler.o: compiler.cpp define.h sym_table.h output.h
	$(CC) compiler.cpp -I $(INCDIR) -c $(OPTS)
expression.o: expression.cpp expression.h define.h translation.h
	$(CC) expression.cpp -I $(INCDIR) -c $(OPTS)
translation.o: translation.cpp translation.h #define.h  sym_table.h
	$(CC) translation.cpp -I $(INCDIR) -c $(OPTS)
sym_table.o: sym_table.cpp define.h sym_table.h
	$(CC) sym_table.cpp -I $(INCDIR) -c $(OPTS)
output.o: output.cpp output.h
	$(CC) output.cpp -I $(INCDIR) -c $(OPTS)

clean:
	rm *.o
