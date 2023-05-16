CFLAGS=-Llib -Iinclude -no-pie 
LIBS=-pthread -ldl -lm -lxcb -lX11 -l:libraylib.a 

# Quando quiser adicionar um módulo novo, coloque
# sempre $(CFLAGS) e $(LIBS) no final!

jogo: main.c
	gcc main.c -o jogo $(CFLAGS) $(LIBS)

# O comando `make run` vai compilar e rodar seu código!
# Não se esqueca de colocar seu módulo depois de `jogo`
run: jogo
	./jogo

# Se quiser recompilar seu jogo do 0, o comando `make clean` vai
# limpar a pasta de artefatos de compilação, etc.
# Não se esqueça de adicionar seu módulo depois de `jogo`
clean:
	rm jogo

.PHONY: run clean

