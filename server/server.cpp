#include	"../util.h"
#include	"../msg_buffer.h"

#include	<sys/epoll.h>

using namespace std;

/*
 *Accept a connection socket and add events to epoll
 *@return 
 	accepted socket id if success
	-1 if fail
 * */
int do_accept(int listen_fd, int epoll_fd)
{
	int conn_fd;

	/* accept a connection socket */
	if ( (conn_fd = accept(listen_fd, NULL, NULL)) < 0)
		return -1;

	if ( make_socket_unblocking(conn_fd) < 0) 
	{
		close(conn_fd);
		return -1;
	}

	/* add events to epoll */
	epoll_event ev;
	create_event(&ev, conn_fd, EPOLLIN);

	if ( epoll_ctl(epoll_fd, EPOLL_CTL_ADD, conn_fd, &ev) < 0)
	{
		close(conn_fd);
		return -1;
	}

	return conn_fd;
}

void show_msg(char* msg, int msg_len)
{
	printf("message content :\n");
	for (int i = 0; i < msg_len; ++ i)
		printf("%d ", msg[i]);
	printf("\n");
}

void epoll_loop(int epoll_fd, int listen_fd)
{
	int n, i;
	epoll_event events[MAX_EVENTS];

	while (1)
	{
		if ( (n = epoll_wait(epoll_fd, events, MAX_EVENTS, -1)) < 0)
			err_quit("epoll_wait call fail.");

		for (i = 0; i < n; ++ i)
		{
			int sock_fd = events[i].data.fd;

			if (sock_fd == listen_fd)
			{
				do_accept(listen_fd, epoll_fd);
			}
			else 
			{
				if (events[i].events & EPOLLIN)
				{
					/* read message and display */
					char msg[BUFF_SIZE];
					int  msg_len;
					Msg_buffer msg_buff(sock_fd, 1024, 4);
					
					int ret = msg_buff.read_all();
					if (ret > 0)
					{ /* read all message and display */
						while ( (msg_len = msg_buff.pop_a_msg(msg)) >= 0)
							show_msg(msg, msg_len);
					}
					else if (ret == 0)
					{ /* client close */
						close(sock_fd);
						printf("client close\n");
					}
					else
					{ /* read socket fail */
						close(sock_fd);
						err_sys("read socket fail");
					}
				}
			}
		}
	}
}

int main()
{
	int listen_fd;
	
	/* create listening socket */
	if ( (listen_fd = create_socket(NULL, PORT_NO)) < 0)
		err_quit("create listening socket fail.");

	if ( make_socket_unblocking(listen_fd) < 0)
		err_quit("make listening socket unblocking fail.");

	listen(listen_fd, 100);
	
	/* create epoll with lt */
	int epoll_fd;
	if ( (epoll_fd = epoll_create(1)) < 0)
		err_quit("create epoll fail.");

	/* add listening socket to epoll */
	epoll_event ev;
	create_event(&ev, listen_fd, EPOLLIN);

	if ( epoll_ctl(epoll_fd, EPOLL_CTL_ADD, listen_fd, &ev) < 0)
		err_quit("add listening socket to epoll fail.");

	/* event polling */
	epoll_loop(epoll_fd, listen_fd);

	return 0;
}
