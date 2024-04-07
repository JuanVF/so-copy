rund:
	gcc -o main main.c files.c process.c linked_list.c tree.c && ./main $(O) $(D) --enable-debug
run:
	gcc -o main main.c files.c process.c tree.c && ./main $(O) $(D)