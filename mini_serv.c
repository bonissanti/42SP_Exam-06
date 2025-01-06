#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>

void ft_putstr(char *str)
{
    int i = 0;
    while (str[i])
    {
        write(2, &str[i], 1);
        i++;
    }
}

typedef struct s_client
{
    int fd;
    char msg[65532];
} t_client;

int id = 0;

int main(int argc, char **argv)
{
    int max_fd = 0;
    struct sockaddr_in server;
    t_client client[1024];

    memset(&server, 0, sizeof(server));
    // memset(&client, 0, sizeof(client));
    server.sin_family = AF_INET;
    server.sin_port = htons(atoi(argv[1]));
    server.sin_addr.s_addr = htonl(INADDR_ANY);

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (bind(server_fd, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
        ft_putstr("Fatal error\n");
        exit(1);
    }

    if (listen(server_fd, 0) < 0)
    {
        ft_putstr("Fatal error\n");
        exit(1);
    }

    max_fd = server_fd;

    fd_set write_fds, read_fds, current_fds;
    FD_ZERO(&read_fds);
    FD_ZERO(&write_fds);
    FD_ZERO(&current_fds);

    FD_SET(server_fd, &current_fds);
    while (1)
    {
        read_fds = current_fds;
        write_fds = current_fds;

        if (select(max_fd + 1, &read_fds, &write_fds, NULL, NULL) < 0)
        {
            ft_putstr("Fatal error\n");
            FD_CLR(server_fd, &current_fds);
            close(server_fd);
            exit(1);
        }
        for (int i = 0; i <= max_fd; i++)
        {
            if (FD_ISSET(i, &read_fds))
            {
                if (i == server_fd)
                {
                    socklen_t len = sizeof(server);
                    const int client_fd = accept(server_fd, (struct sockaddr *)&server, &len);
                    if (client_fd < 0)
                    {
                        ft_putstr("Fatal error\n");
                        FD_CLR(client_fd, &current_fds);
                        FD_CLR(server_fd, &current_fds);
                        close(server_fd);
                        exit(1);
                    }
                    FD_SET(client_fd, &current_fds);
                    // client[id++].fd = client_fd;
                    break ;
                }
                ft_putstr("Hello\n");
            }
        }

    }
}