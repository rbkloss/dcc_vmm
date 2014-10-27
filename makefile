all:
	gcc -g vmm.c vmm_ext.c disk.c swap.c os_init.c main.c -o vmm.exe
clean:
	rm *.o *.exe *~
