# Compilador e Flags
CC = gcc
CFLAGS = -Wall -Wextra -g

# Nome da pasta de saída
ODIR = Compilados

# Lista de Headers (Dependências)
# Se mudar qualquer .h, ele recompila tudo para garantir segurança
DEPS = fila_fifo.h logtree.h escalonador.h

# Lista de Objetos com o caminho da pasta
# Isso diz: main.o vira Compilados/main.o, etc.
_OBJ = main.o fila_fifo.o logtree.o escalonador.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

# Regra Principal (Default)
all: create_dir $(ODIR)/simulador

# Regra para criar a pasta se ela não existir (-p evita erro se já existir)
create_dir:
	@mkdir -p $(ODIR)

# Regra de Linkagem (Gera o executável final DENTRO da pasta)
# $@ significa o alvo (Compilados/simulador)
# $^ significa todas as dependências (os .o)
$(ODIR)/simulador: $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

# Regra de Compilação Genérica
# Transforma qualquer .c da raiz em um .o dentro da pasta Compilados
# $< significa o arquivo .c de origem
$(ODIR)/%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $<

# Limpeza (Apaga a pasta inteira)
clean:
	rm -rf $(ODIR)