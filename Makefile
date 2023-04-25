proj2: proj2.c
	gcc -pthread -std=gnu99 -Wall -Wextra -Werror -pedantic proj.c -o proj2

clean:
	rm *.o