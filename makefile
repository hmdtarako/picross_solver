demo: demo.c analysis display solver
	gcc -o demo demo.c analysis.o display.o solver.o

analysis: analysis.c
	gcc -c -o analysis.o analysis.c

display: display.c
	gcc -c -o display.o display.c

solver: solver.c
	gcc -c -o solver.o solver.c

.PHONY: clean
clean:
	rm -f *.o
	rm -f demo
