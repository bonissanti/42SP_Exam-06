#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>

int max(int a, int b)
{
    if (a > b)
        return a;
    return b;
}

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
    char buffer[65526];
    // char sendBuffer[65526];
    int maxFd;
    int server_fd;
    struct sockaddr_in server;

    if (argc != 2)
    {
        ft_putstr_fd(2, "Wrong number of arguments\n");
        return 1;
    }
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(atoi(argv[1]));
    server.sin_addr.s_addr = htonl(INADDR_ANY);

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        ft_putstr_fd(2, "Fatal error\n");
        return 1;
    }
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
    {
        ft_putstr_fd(2, "Fatal error\n");
        return 1;
    }

    if (bind(server_fd, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
        ft_putstr_fd(2, "Fatal error\n");
        return 1;
    }


    if (listen(server_fd, 100) == -1)
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
    FD_SET(server_fd, &current_fds);
    maxFd = server_fd;
    while (true)
    {
        read_fds = current_fds;
        write_fds = current_fds;
        if (select(FD_SETSIZE, &read_fds, &write_fds, NULL, NULL) < 0)
        {
            ft_putstr_fd(2, "Fatal error\n");
            return 1;
        }
        for (int i = 0; i < maxFd; i++)
        {
            if (FD_ISSET(i, &read_fds))
            {
                if (i == server_fd)
                {
                    socklen_t len = sizeof(server);
                    const int client_fd = accept(i, (struct sockaddr *)&server, &len);
                    if (client_fd < 0)
                        continue;
                    FD_SET(client_fd, &current_fds);
                    maxFd = max(maxFd, client_fd);
                    ft_putstr_fd(2, "Server connected\n");
                }
                else
                {
                    ssize_t len = recv(i , buffer,sizeof(buffer), 0);
                    for (int fd = 0; fd < maxFd; fd++)
                    {
                        if (FD_ISSET(fd, &write_fds) && fd != i)
                        {
                            if (send(fd, buffer, len, 0) < 0)
                            {
                                ft_putstr_fd(2, "Fuck you\n");
                                return 1;
                            }
                        }
                    }
                }
            }
        }
    }
    return 0;
}