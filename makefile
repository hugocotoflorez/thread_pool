all:
	gcc tp.c -lm -fsanitize=address,null -g
