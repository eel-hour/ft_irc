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
    int cliSock;
    int cliID;
    std::string nickName;
    std::string userName;
    std::string passWord;
    std::string channels;
};
int bufferSize(char *buffer){
    int p = 0;
    while(buffer[p])
        p++;
    return p;
}

std::vector<std::string> split(std::string s, std::string delimiter) {
    size_t pos_start = 0, pos_end, delim_len = delimiter.length();
    std::string token;
    std::vector<std::string> res;

    while ((pos_end = s.find(delimiter, pos_start)) != std::string::npos) {
        token = s.substr (pos_start, pos_end - pos_start);
        pos_start = pos_end + delim_len;
        res.push_back (token);
    }

    res.push_back (s.substr (pos_start));
    return res;
}

void _nick(std::vector<std::string> buffer, int i, std::vector<struct data> db){
    std::string tmp = buffer[1];
    for (int j = 0; j < db.size(); j++){
        if (db[j].cliSock == fds[j].fd)
            db[j].nickName = tmp;
    }
}

void _user(std::vector<std::string> buffer, int i, std::vector<struct data> db){
    std::string tmp = buffer[3];
    for (int j = 0; j < db.size(); j++){
        if (db[j].cliSock == fds[j].fd)
            db[j].userName = tmp;
    }
}

void _pass(std::vector<std::string> buffer, int i, std::vector<struct data> db){
    std::string tmp = buffer[1];
    for (int j = 0; j < db.size(); j++){
        if (db[j].cliSock == fds[j].fd)
            db[j].passWord = tmp;
    }
}

void parse(std::string buffer, int i, std::vector<struct data> db){
    std::vector<std::string> splited = split(buffer, " ");
    if (splited.size() > 1){
        if (splited[0] == "NICK" && splited.size() == 2)
            _nick(splited, i, db);
        else if (splited[0] == "USER" && splited.size() == 4)
            _user(splited, i, db);
        else if (splited[0] == "PASS" && splited.size() == 2)
            _pass(splited, i, db);
        else
            std::cout << "THE COMMAND DOES NOT EXIST." << std::endl;
    }
    else
        std::cout << "THE COMMAND DOES NOT EXIST." << std::endl;
}


void listenForClients(){


}

int main(int ac, char **av){
    if (ac != 2){
        std::cout << "Enter correct arguments." << std::endl;
        return 1;
    }
    int servSock = socket(AF_INET, SOCK_STREAM, 0);
    if (servSock < 0)
    {
        std::cout << "SOMETHING BAD HAPPENED IN SOCKET" << std::endl;
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
        std::cout << "SOMETHING BAD HAPPENED IN BIND" << std::endl;
        exit(0);
    }
    int list = listen(servSock, 5);
    if (list) {
        std::cout << "SOMETHING BAD HAPPENED IN LISTEN" << std::endl;
        exit(0);
    }
    unsigned int leng;
    leng = sizeof(client); 
    char buffer[1024] = {0};
    fcntl(servSock, F_SETFL, O_NONBLOCK);
    int i = 0;
    int p;
    std::vector<struct data> db;
    while (1) {
        int cliSock = accept(servSock, (struct sockaddr *)&client, &leng);
        if (cliSock > 0)
        {
            struct data data_struct;
            data_struct.cliSock = cliSock;
            db.push_back(data_struct);
            struct pollfd fd;
            fd.fd = cliSock;
            fd.events = POLLIN;
            fd.revents = 0;
            fds.push_back(fd);
            i++;
        }
        p = poll(fds.data(), fds.size(), 10);
        if (p > 0)
        {
            for (size_t i = 0; i < fds.size(); i++)
            {
                int leng = recv(fds[i].fd, buffer, sizeof(buffer), 0);
                buffer[leng] = 0;
                std::string cover(buffer);
                // std::cout << buffer;
                parse(cover, i, db);
                buffer[0] = '\0';
            }
        }
    }
}
