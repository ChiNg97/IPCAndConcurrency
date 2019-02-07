s-talk:	main.o list.o
	gcc -Wall -Werror -std=c99 -o s-talk -pthread main.o list.o 
main.o:	main.c 
	gcc -Wall -Werror -std=c99 -c main.c
list.o: list.c list.h
	gcc -Wall -Werror -std=c99 -c list.c	
clean:
	rm main.o s-talk list.o