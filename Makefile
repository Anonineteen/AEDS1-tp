CFLAGS=-Llib -Iinclude -no-pie -g
LIBS=-l:libraylib.a -pthread -ldl -lm -lxcb -lX11

# Quando quiser adicionar um módulo novo, coloque
# sempre $(CFLAGS) e $(LIBS) no final!

jogo: main.c tiro.o inimigos.o coisas.h
	gcc main.c tiro.o inimigos.o -o jogo $(CFLAGS) $(LIBS)

tiro.o: tiro.c tiro.h
	gcc -c tiro.c $(CFLAGS) $(LIBS)

inimigos.o: inimigos.c inimigos.h
	gcc -c inimigos.c $(CFLAGS) $(LIBS)

# O comando `make run` vai compilar e rodar seu código!
# Não se esqueca de colocar seu módulo depois de `jogo`
run: jogo
	./jogo

# Se quiser recompilar seu jogo do 0, o comando `make clean` vai
# limpar a pasta de artefatos de compilação, etc.
# Não se esqueça de adicionar seu módulo depois de `jogo`
clean:
	rm jogo *.o

.PHONY: run clean

