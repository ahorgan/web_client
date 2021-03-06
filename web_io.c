#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

#define RECV_LEN 256
#define RECV_BYTES 8

int main(int argc, char **argv)
{
	struct addrinfo hints;
	struct addrinfo *result;
	char *host = "farnsworth.ecst.csuchico.edu";
	char *port = "80";
	int farn_socket;
	int s;
	char *msg = "GET /lab_docs/reset_instructions.pdf HTTP/1.0\n\n";
	
	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	hints.ai_protocol = 0;
	hints.ai_canonname = NULL;
	hints.ai_addr = NULL;
	hints.ai_next = NULL;
	
	if((s = getaddrinfo(host, port, &hints, &result)) != 0)
	{
		perror("getaddressinfo");
		return -1;
	}
	
	farn_socket = -1;
	if((farn_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		perror("socket");
		return -1;
	}
	
	if(connect(farn_socket, result->ai_addr, result->ai_addrlen) == -1)
	{
		perror("Error connecting");
		return -1;
	}
	
	freeaddrinfo(result);
	
	send(farn_socket, msg, strlen(msg), 0);
	
	char recv_msg[RECV_LEN];
	int recv_flag;
	int fd = open("local_file", O_WRONLY | O_APPEND | O_CREAT | 
O_TRUNC, S_IRWXU);
	memset(recv_msg, 0, strlen(recv_msg));

	while((recv_flag = recv(farn_socket, recv_msg, RECV_LEN, 0)) > 0)
	{	
		if(write(fd, recv_msg, recv_flag) <=  0)
		{
			perror("Error writing to file: ");
			return 1;
		}
        memset(recv_msg, 0, strlen(recv_msg));
	}
	if(recv_flag < 0)
	{
		perror("Error receiving bytes");
		return 1;
	}
	
    close(fd);
	close(farn_socket);
	return 0;
}

