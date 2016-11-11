objs = genlist.o taskqueue.o pthreadpool.o serroutine.o databasectl.o server.o
objc = client.o user_func.o pthreadpool.o taskqueue.o genlist.o

all:../output/server_app ../output/client_app

../output/server_app:$(objs)
	gcc $^ -o $@  -lpthread -lsqlite3


../output/client_app:$(objc)
	gcc $^ -o $@ -lpthread
