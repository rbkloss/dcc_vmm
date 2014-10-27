all:
<<<<<<< HEAD
	gcc -g vmm.c vmm_ext.c main.c -o vmm.exe
clean:
	rm *.o *.exe *~
=======
	gcc -g vmm.c vmm_ext.c swap.c os_init.c main.c -o vmm.exe
clean:
	rm *.o *.exe *~
>>>>>>> 4e3266549d5782a9f98bcfe04d86264715f44f67
