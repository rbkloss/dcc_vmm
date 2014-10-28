all:
	gcc -g -std=gnu99 -Wall vmm.c vmm_ext.c disk.c part5.c swap.c os_init.c main.c -o vmm.exe
clean:
	rm *.o *.exe *~
