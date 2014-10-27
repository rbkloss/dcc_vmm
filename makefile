all:
	gcc -g vmm.c vmm_ext.c swap.c os_init.c part5.c main.c -o vmm.exe
clean:
	rm *.o *.exe *~
