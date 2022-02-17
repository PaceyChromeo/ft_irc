#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <iostream>
#include <string.h>
#include <stdlib.h>
#include "ircserv.hpp"

typedef struct  s_user
{
    char name[30];
    int age;

} t_user;

int main(int ac, char **av) {

    if (ac != 2){
    	std::cerr << "1 argument required : ./ircserv [ PORT ]\n";
    	exit (1);
    }
    int port = atoi(av[1]);
    struct sockaddr_in addrServer;
    struct sockaddr_in addrClient;
    int sd_server = socket(AF_INET, SOCK_STREAM, 0);
    if (sd_server < 0)
    {
        std::cerr << "Failed to create listening socket" << std::endl;
        exit(1);
    }
    addrServer.sin_family = AF_INET;
    addrServer.sin_addr.s_addr = inet_addr("127.0.0.1");
    addrServer.sin_port = htons(port);
    if (bind(sd_server, (const struct sockaddr *) &addrServer, sizeof(addrServer)) < 0) {
        std::cerr << "Bind Error : " << strerror(errno) << std::endl;
        close(sd_server);
        exit(1);
    }
    std::cout << "socket server : " << sd_server << std::endl;
    if (listen(sd_server, 15) < 0)
    {
        std::cerr << "Listen error : " << strerror(errno) << std::endl;
		close(sd_server);
		exit (1);
	}
	socklen_t csize = sizeof(addrClient);
	int sd_client = accept(sd_server, (struct sockaddr *)&addrClient, &csize);
	if (sd_client < 0) {
		std::cerr << "Accept Error : " << strerror(errno) << std::endl;
		close(sd_server);
		exit(1);
	}
	char msg[1000];
	ssize_t octet_recv = recv(sd_client, msg, 1000, 0);
	if (octet_recv < 0){
		std::cerr << "Nothing to receive\n";
		close(sd_server);
		close(sd_client);
		exit(1);
	}
    while (octet_recv > 0) {
        std::cout << "Message recu : " << msg;
    	memset(msg, 0, strlen(msg));
		octet_recv = recv(sd_client, msg, 1000, 0);
    }
    close(sd_client);
    close(sd_server);
    return 0;
}