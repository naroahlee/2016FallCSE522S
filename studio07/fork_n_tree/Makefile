.PHONY: all clean

all: fork tree

clean:
	rm -rf *.o fork tree

fork: fork.o
	gcc fork.o -o fork

fork.o: fork.c
	gcc fork.c -c -o fork.o

tree: tree.o
	gcc tree.o -o tree

tree.o: tree.c
	gcc tree.c -c -o tree.o
