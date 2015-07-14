obj=client.o msg_buffer.o

client:$(obj)
	g++ -o client $(obj)

client.o:client.c
	g++ -c client.c

msg_buffer.o:../msg_buffer.cpp
	g++ -c ../msg_buffer.cpp

clean:
	rm client $(obj)
