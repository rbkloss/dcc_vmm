all:
	gcc -g vmm.c vmm_ext.c swap.c os_init.c main.c -o vmm.exe
clean:
	rm *.o *.exe *~
