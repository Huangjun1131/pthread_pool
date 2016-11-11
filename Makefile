CFLAGS=-DEBUG -I../include -Iinclude

export CFLAGS

all:
	make -C genlist
	make -C taskqueue
	make -C pthreadpool
	make -C databasectl
	make -C serroutine
	make -C server
	make -C user_func
	make -C client
	make -C obj
clean:
		rm obj/*.o output/*pp

