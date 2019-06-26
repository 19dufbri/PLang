IDIR   = ./include
ODIR   = ./bin
SDIR   = ./src
OUT    = bint
CFLAGS = -std=c11 -I$(IDIR) -Wall
CC     = gcc
HEAD   = $(wildcard $(IDIR)/*.h)
OBJ    = $(ODIR)/interpreter.o $(ODIR)/linked_list.o $(ODIR)/mapper.o 

all: $(OUT)

$(ODIR)/%.o: $(SDIR)/%.c $(HEAD) $(ODIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OUT): $(OBJ)
	$(CC) $(CFLAGS) $^ -o $@

$(ODIR):
	mkdir $(ODIR)

run: all
	./$(OUT)

install: all
	cp $(OUT) /usr/local/bin/$(OUT)

clean:
	rm -rf $(OUT) $(ODIR)