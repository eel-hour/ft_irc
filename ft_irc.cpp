#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cstdlib>
#include <thread>
#include <fcntl.h>
#include <vector>
#include <poll.h>

std::vector<struct pollfd> fds;

struct data{
    int cliNumb;

};
int bufferSize(char *buffer){
    int p = 0; 
    while(buffer[p])
        p++;
    return p;
}


void listenForClients(){


}

int main(int ac, char **av){
    // if (ac != 3){
    //     std::cout << "Enter correct arguments." << std::endl;
    //     return 1;}

    int servSock = socket(AF_INET, SOCK_STREAM, 0);
    if (servSock < 0)
    {
        printf("SOMETHING BAD HAPPENED IN SOCKET\n");
        exit(0);
    }
    struct sockaddr_in servAddr;
    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons(atoi(av[1]));
    servAddr.sin_addr.s_addr = INADDR_ANY;
    struct sockaddr_in client;
    int bin = bind(servSock, (struct sockaddr *)&servAddr, sizeof(servAddr));
    if (bin < 0)
    {
        printf("SOMETHING BAD HAPPENED IN BIND\n");
        exit(0);
    }
    int list = listen(servSock, 5);
    if (list) {
        printf("SOMETHING BAD HAPPENED IN LISTEN\n");
        exit(0);
    }
    unsigned int *leng;
    *leng = sizeof(client); 
    char buffer[1024] = {0};
    fcntl(servSock, F_SETFL, O_NONBLOCK);
    int i = 0;
    int p;
    while (1) {
        int cliSock = accept(servSock, (struct sockaddr *)&client, leng);
        if (cliSock > 0)
        {
            struct pollfd fd;
            fd.fd = cliSock;
            fd.events = POLLIN;
            fd.revents = 0;
            fds.push_back(fd);

            i++;
            printf("%d\n", cliSock);
        }
        p = poll(fds.data(), fds.size(), 10);
        if (p > 0)
        {
            for (size_t i = 0; i < fds.size(); i++)
            {
                int leng = recv(fds[i].fd, buffer, sizeof(buffer), 0);
                buffer[leng] = 0;
                printf("%s\n", buffer);
                buffer[0] = '\0';
            }
        }
        // int po = poll(fds, fds.size(), 15);
        // if (po)
        // {
        //     printf("wtf");
        //     for (size_t i = 0; i < fds.size(); i++)
        //     {
        //         int leng = recv(cliSock, buffer, sizeof(buffer), 0);
        //         // int re = read(fds[i], buffer, sizeof(buffer));
        //         if (leng)
        //         {
        //             printf("%s\n", buffer);
        //         }
        //     }
        // }
        // buffer[leng] = '\0';
        // if (buffer[0])
        //     printf("%s\n", buffer);
        // send(cliSock, buffer, sizeof(buffer), 0);
        // recv(cliSock, buffer, sizeof(buffer), 0);
        // if (bufferSize(buffer))
        //     std::cout << "Message: " << std::endl;
    }
    // close(servSock);
}
