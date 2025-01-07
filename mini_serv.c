#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

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
int maxFd = 0;
char recvBuf[65632];
t_client client[1024];
fd_set readFds, writeFds, currentFds;

void send_to_all(int exceptFd)
{
    for (int i = 0; i <= maxFd; i++)
    {
        if (FD_ISSET(i, &writeFds) && i != exceptFd)
            send(i, recvBuf, sizeof(recvBuf), 0);
    }
}

void broadcast_message(t_client client[1024], char buffer[65532], int fd, ssize_t len)
{
    for (int i = 0; i < len; i++)
    {
        client[fd].msg[i] = buffer[i];
        if (client[fd].msg[i] == '\n')
        {
            client[fd].msg[i] = '\0';
            sprintf(recvBuf, "client %d: %s\n", client[fd].id, client[fd].msg);
            send_to_all(fd);
            bzero(&recvBuf, strlen(recvBuf));
            bzero(&client[fd].msg, strlen(client[fd].msg));
        }
    }
}

int main(int argc, char **argv)
{
    char buffer[65532];
    struct sockaddr_in server = {0};

    if (argc != 2)
    {
        ft_putstr("Wrong number of arguments\n");
        return 1;
    }

    memset(&client, 0, sizeof(t_client));
    const int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    server.sin_family = AF_INET;
    server.sin_port = htons(atoi(argv[1]));
    server.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(server_fd, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
        ft_putstr("Fatal error\n");
        close(server_fd);
        exit(1);
    }
    if (listen(server_fd, 10) < 0)
    {
        ft_putstr("Fatal error\n");
        close(server_fd);
        exit(1);
    }
    maxFd = server_fd;
    FD_ZERO(&readFds);
    FD_ZERO(&writeFds);
    FD_ZERO(&currentFds);

    FD_SET(server_fd, &currentFds);
    while (1)
    {
        writeFds = currentFds;
        readFds = currentFds;
        if (select(maxFd + 1, &readFds, &writeFds, NULL, NULL) < 0)
        {
            ft_putstr("Fatal error\n");
            close(server_fd);
            exit(1);
        }
        for (int i = 0; i <= maxFd; i++)
        {
            if (FD_ISSET(i, &readFds))
            {
                if (i == server_fd)
                {
                    socklen_t len = sizeof(server);
                    int client_fd = accept(server_fd, (struct sockaddr *)&server, &len);
                    if (client_fd < 0)
                    {
                        ft_putstr("Fatal error\n"); // Maybe don't need to be printed
                        continue;
                    }
                    client[client_fd].id = id++;
                    maxFd = max(maxFd, client_fd);
                    FD_SET(client_fd, &currentFds);
                    sprintf(recvBuf, "server: client %d just arrived\n", client[client_fd].id);
                    send_to_all(client_fd);
                    bzero(&recvBuf, strlen(recvBuf));
                    break;
                }
                const ssize_t len = recv(i, buffer, sizeof(buffer), 0);
                if (len <= 0)
                {
                    sprintf(recvBuf, "server: client %d just left\n", client[i].id);
                    send_to_all(i);
                    FD_CLR(i, &currentFds);
                    close(i);
                    bzero(&recvBuf, strlen(recvBuf));
                    break ;
                }
                buffer[len] = '\0';
                broadcast_message(client, buffer, i, len);
                bzero(&buffer, sizeof(buffer));
            }
        }
    }
}