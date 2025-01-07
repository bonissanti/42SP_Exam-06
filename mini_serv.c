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

typedef struct s_client
{
    int id;
    char msg[65532];
} t_client;

int maxFd = 0;
int id = 0;
char revBuf[65632];
fd_set readFds, writeFds, currentFds;
t_client client[1024];

void send_to_all(int exceptFd)
{
    for (int i =0; i <= maxFd; i++)
    {
        if (FD_ISSET(i, &writeFds) && i != exceptFd)
        {
            if (send(i, revBuf, strlen(revBuf), 0) < 0)
            {
                ft_putstr("Fatal error\n");
                exit(1);
            }
        }
    }
}

int main(int argc, char **argv)
{
    int server_fd = 0;
    struct sockaddr_in server;

    if (argc != 2)
    {
        ft_putstr("Wrong number of arguments\n");
        return (1);
    }

    memset(&server, 0, sizeof(server));
    memset(&client, 0, sizeof(t_client));

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = htons(atoi(argv[1]));

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

    FD_ZERO(&readFds);
    FD_ZERO(&writeFds);
    FD_ZERO(&currentFds);
    FD_ISSET(server_fd, &currentFds);
    maxFd = server_fd;
    while(1)
    {
        writeFds = currentFds;
        readFds = currentFds;
        if (select(maxFd + 1, &readFds, &writeFds, NULL, NULL) < 0)
        {
            ft_putstr("Fatal error\n");
            close(server_fd);
            FD_CLR(server_fd, &currentFds);
            exit(1);
        }
        for (int i = 0; i <= maxFd; i++)
        {
            if (FD_ISSET(server_fd, &readFds))
            {
                if (i == server_fd)
                {
                    socklen_t len = sizeof(server);
                    int client_fd = accept(server_fd, (struct sockaddr *)&server, &len);
                    if (client_fd < 0)
                    {
                        ft_putstr("Fatal error\n");
                        break ;
                    }
                    client[client_fd].id = id++;
                    FD_SET(client_fd, &currentFds);
                    sprintf(revBuf, "server: client %d just arrived\n", client[client_fd].id);
                    send_to_all(client_fd);
                    bzero(&revBuf, strlen(revBuf));
                }
            }
        }

    }
}