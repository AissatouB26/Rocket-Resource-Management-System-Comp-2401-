OPT = -Wall -Wextra -pthread
OBJ = main.o event.o manager.o resource.o system.o

program: $(OBJ)
	gcc $(OPT) -o program $(OBJ)
main.o: main.c defs.h
	gcc $(OPT) -c main.c

event.o: event.c defs.h
	gcc $(OPT) -c event.c

manager.o: manager.c defs.h
	gcc $(OPT) -c manager.c

resource.o: resource.c defs.h
	gcc $(OPT) -c resource.c

system.o: system.c defs.h
	gcc $(OPT) -c system.c

clean:
	rm -f $(OBJ) program

run: program
	./program
