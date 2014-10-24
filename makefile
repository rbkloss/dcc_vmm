all:
	gcc -g vmm.c vmm_ext.c main.c -o vmm.exe
clean:
	rm *.o *.exe *~