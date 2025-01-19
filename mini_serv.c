#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void ft_putstr(char *str)
{
    int i = 0;
    while (str[i])
    {
        write(2, &str[i], 1);
        i++;
    }
}

int getMax(int a, int b)
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
int maxFd = 0;
t_client client[1024];
char send_buffer[65632];
char recv_buffer[65532];
fd_set write_fds, read_fds, current_fds;

void send_to_all(int exceptFd)
{
    for (int i = 0; i <= maxFd; i++)
    {
        if (FD_ISSET(i, &write_fds) && i != exceptFd)
            send(i, send_buffer, strlen(send_buffer), 0);
    }
}

void broadcast_message(t_client client[1024], char recv_buffer[65532], int fd, ssize_t len)
{
    int j = strlen(client[fd].msg);
    for (int i = 0; i < len; i++, j++)
    {
        client[fd].msg[j] = recv_buffer[i];
        if (client[fd].msg[j] == '\n')
        {
            client[fd].msg[j] = '\0';
            sprintf(send_buffer, "client %d: %s\n", client[fd].id, client[fd].msg);
            send_to_all(fd);
            bzero(&send_buffer, strlen(send_buffer));
            bzero(&client[fd].msg, strlen(client[fd].msg));
            j = -1;
        }
    }
}

int main(int argc, char **argv)
{
    struct sockaddr_in server;

    if (argc != 2)
    {
        ft_putstr("Wrong number of arguments\n");
        return 1;
    }

    memset(&server, 0, sizeof(server));
    memset(&client, 0, sizeof(t_client));

    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = htons(atoi(argv[1]));
    server.sin_family = AF_INET;

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (bind(server_fd, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
        ft_putstr("Fatal error\n");
        close(server_fd);
        return 1;
    }

    if (listen(server_fd, 10) < 0)
    {
        ft_putstr("Fatal error\n");
        close(server_fd);
        return 1;
    }

    FD_ZERO(&write_fds);
    FD_ZERO(&read_fds);
    FD_ZERO(&current_fds);

    FD_SET(server_fd, &current_fds);
    maxFd = server_fd;

    while (1)
    {
        read_fds = current_fds;
        write_fds = current_fds;

        if (select(maxFd + 1, &read_fds, &write_fds, NULL, NULL) < 0)
        {
            ft_putstr("Fatal error\n");
            close(server_fd);
            FD_CLR(server_fd, &current_fds);
            return 1;
        }

        for (int i = 0; i <= maxFd; i++)
        {
            if (FD_ISSET(i, &read_fds))
            {
                if (i == server_fd)
                {
                    socklen_t len = sizeof(server);
                    int client_fd = accept(server_fd, (struct sockaddr *)&server, &len);
                    if (client_fd < 0)
                        continue;
                    maxFd = getMax(maxFd, client_fd);
                    FD_SET(client_fd, &current_fds);
                    client[client_fd].id = id++;
                    sprintf(send_buffer, "server: client %d just arrived\n", client[client_fd].id);
                    send_to_all(client_fd);
                    bzero(&send_buffer, strlen(send_buffer));
                    break;
                }
                else
                {
                    ssize_t len = recv(i, recv_buffer, sizeof(recv_buffer), 0);
                    if (len <= 0)
                    {
                        bzero(&send_buffer, strlen(send_buffer));
                        sprintf(send_buffer, "server: client %d just left\n", client[i].id);
                        send_to_all(i);
                        FD_CLR(i, &current_fds);
                        close(i);
                        bzero(&client[i].msg, strlen(client[i].msg));
                        break;
                    }
                    else
                    {
                        recv_buffer[len] = '\0';
                        broadcast_message(client, recv_buffer, i, len);
                    }
                }
            }
        }
    }
}