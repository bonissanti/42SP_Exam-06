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

int max(int a, int b)
{
    if (a > b)
        return a;
    return b;
}

typedef struct s_client
{
    int id;
    char msg[65532];
} t_client;

int id = 0;
int max_fd = 0;
char recvBuf[65632];
fd_set write_fds, read_fds, current_fds;

void send_to_all(int exceptFd)
{
    for (int i = 0; i <= max_fd; i++)
    {
        if (FD_ISSET(i, &write_fds) && i != exceptFd)
        {
            if (send(i, recvBuf, sizeof(recvBuf), 0) < 0)
            {
                ft_putstr("Fatal error\n");
                FD_CLR(i, &write_fds);
                close(i);
            }
        }
    }
}

void broadcast_message(t_client *client, int fd, char msg[65532], ssize_t len)
{
    for (int i = 0; i < len; i++)
    {
        client[fd].msg[i] = msg[i];
        if (client[fd].msg[i] == '\n')
        {
            client[fd].msg[i] = '\0';
            //TODO: write in a buffer using sprintf
            //TODO: delivery to all fds connected, except sender (using fd_isset - writeFds)
            sprintf(recvBuf, "client %d: %s\n", client[fd].id, client[fd].msg);
            // ft_putstr(client[fd].msg);
            send_to_all(fd);
            bzero(&msg, strlen(msg));
        }
    }
}


int main(int argc, char **argv)
{
    char buffer[65532];
    struct sockaddr_in server;
    t_client *client = NULL;

    if (argc != 2)
    {
        ft_putstr("Wrong number of arguments\n");
        return 1;
    }

    client = calloc(2048, sizeof(t_client));
    if (!client)
    {
        ft_putstr("Fatal error\n");
        return 1;
    }
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(atoi(argv[1]));
    server.sin_addr.s_addr = htonl(INADDR_ANY);

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (bind(server_fd, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
        ft_putstr("Fatal error\n");
        free(client);
        exit(1);
    }

    if (listen(server_fd, 0) < 0)
    {
        ft_putstr("Fatal error\n");
        free(client);
        exit(1);
    }

    max_fd = server_fd;

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
            free(client);
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
                        free(client);
                        exit(1);
                    }
                    FD_SET(client_fd, &current_fds);
                    client[client_fd].id = id++;
                    max_fd = max(max_fd, client_fd);
                    break ;
                }
                ssize_t len = recv(i, buffer, sizeof(buffer), 0);
                if (len < 0)
                {
                    ft_putstr("Remove client\n");
                    break;
                }
                buffer[len] = '\0';
                broadcast_message(client, i, buffer, len);
            }
        }
    }
    free(client);
}