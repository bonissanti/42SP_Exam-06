#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>

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
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(atoi(argv[1]));
    server.sin_addr.s_addr = INADDR_ANY;

    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if ((bind(socket_fd, (struct sockaddr *)&server, sizeof(server.sin_addr))))
    {
        ft_putstr_fd(2, "Fatal error\n");
        return 1;
    }
    if (listen(socket_fd, 100))
    {
        ft_putstr_fd(2, "Fatal error\n");
        return 1;
    }

    fd_set read_fds;
    fd_set write_fds;
    fd_set current_fds;

    FD_ZERO(&read_fds);
    FD_ZERO(&write_fds);
    FD_ZERO(&current_fds);
    FD_SET(socket_fd, &read_fds);

    while (true)
    {
        current_fds = read_fds;
        if (select(FD_SETSIZE, &current_fds, NULL, NULL, NULL) < 0)
        {
            ft_putstr_fd(2, "Fatal error\n");
            return 1;
        }
        for (int i = 0; i < FD_SETSIZE; i++)
        {
            if (FD_ISSET(i, &current_fds))
            {
                if (i == socket_fd)
                {
                    int client_fd =
                }

            }
        }
    }
    return 0;

}