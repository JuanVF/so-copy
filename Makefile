rund:
	gcc -o main main.c files.c process.c linked_list.c tree.c && ./main $(O) $(D) --enable-debug
run:
	gcc -o main main.c files.c process.c tree.c && ./main $(O) $(D)

clean:
	ipcs -q | awk '/0x/ {print $2}' | xargs -n 1 ipcrm -q