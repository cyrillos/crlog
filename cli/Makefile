target			+= cli
target			+= cli-cprintf

cli-obj-y		+= main.o
cli-cprintf-obj-y	+= main-crlog.o
CFLAGS_main-crlog.o	:= -fplugin=./cprintf/cprintf.so		\
			-fplugin-arg-cprintf-log_level=debug		\
			-fplugin-arg-cprintf-printf="fprintf(1):	\
			%c __putc %s __puts %li __putlong		\
			%d __putint %lu __putulong %% __putwrite"
