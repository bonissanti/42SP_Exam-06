#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>

void ft_putstr_fd(int fd, const char *str)
{
    int i = 0;
    while (str[i])
    {
        write(1, &str[i], 1);
        i++;
    }
}

int main(const int argc, char **argv)
{
    struct sockaddr_in server;
    int socket_fd;

    if (argc != 2)
    {
        ft_putstr_fd(2, "Wrong number of arguments\n");
        return 1;
    }

    memset(&server,0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(atoi(argv[1]));
    server.sin_addr.s_addr = htonl(INADDR_ANY);

    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
}