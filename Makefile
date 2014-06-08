file.run: file.c
	gcc -o file.run file.c
clean:
	rm file.run
