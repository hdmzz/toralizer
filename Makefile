all:
	rm toralize.so
	gcc -Wextra -Wall -Werror main.c -o toralize.so -fPIC -shared -ldl -D_GNU_SOURCE
