# Exemplo de Makefile
CC = gcc
CFLAGS = -Wall -Wextra -g # Flags de aviso e debug
OBJ = main.o fila_fifo.o logtree.o escalonador.o

# Regra principal
all: simulador

# Ligação (Linking)
simulador: $(OBJ)
	$(CC) $(CFLAGS) -o simulador $(OBJ)

# Compilação dos objetos
main.o: main.c escalonador.h
	$(CC) $(CFLAGS) -c main.c

fila_fifo.o: fila_fifo.c fila_fifo.h
	$(CC) $(CFLAGS) -c fila_fifo.c

logtree.o: logtree.c logtree.h
	$(CC) $(CFLAGS) -c logtree.c

escalonador.o: escalonador.c escalonador.h fila_fifo.h logtree.h
	$(CC) $(CFLAGS) -c escalonador.c

# Limpeza
clean:
	rm -f *.o simulador