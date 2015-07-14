#include	"../util.h"
#include	"../msg_buffer.h"

#include	<netinet/in.h>
#include	<sys/types.h>

void conmunicate(int fd) {
	char	msg[BUFF_SIZE];
	int		n_write;
	
	/* create message content with length equals 10*/
	int i;
	for (i = 0; i < 10; ++ i)
		msg[i] = i + 1;
	
	/* create msg_buffer */
	Msg_buffer msg_buff(fd, 1024, 4);
	
	/* send message */
	if (msg_buff.push_a_msg(msg, 10) == 0) {
		i = msg_buff.write_all();
		if (i > 0)
			printf("succeed to write all bytes into socket.\n");
		else if (i == 0)
			printf("socket is full.\n");
		else
			err_sys("system error");
	}
	else
		printf("push message fail.\n");
}

int main(int argc, char** argv) {
	
	int 				conn_fd;
	struct sockaddr_in 	serv_addr;

	if (argc < 2)
		err_quit("require two parameters for main()!");

	if ( (conn_fd = create_socket(argv[1], PORT_NO)) < 0)
		err_quit("create connection socket fail.");

	/* conmunicate with server */
	conmunicate(conn_fd);

	if ( close(conn_fd) == -1)
		err_quit("call close failed!");

	return 0;
}
