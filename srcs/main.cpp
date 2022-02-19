
#include "../includes/Server.hpp"
#include "../includes/Utils.hpp"
#include <string.h>
#include <string>
#include <fcntl.h>

std::string get_nickname(char *msg) {

    std::string str(msg);
    int  find = str.find("K ");
    int find_endl = str.substr(find+2, str.size()).find("\n");
    std::string nickname = str.substr(find+2, find_endl);
    nickname.erase(nickname.size() - 1);

    return nickname;
}

std::string get_username(char *msg) {

    char **tab = ft_split(msg, ' ');
    char *tmp = ft_substr(tab[4], 1, (strlen(tab[4])));
    std::string username(tmp);
    free_tab(tab);
    free(tmp);
    return username;
}

int main(int ac, char **av) {

    if (ac != 2){
        std::cerr << "1 argument required : ./ircserv [ PORT ]\n";
        exit (1);
    }
    int port = atoi(av[1]);
	Server	newServ(port, "toto");
    int client;
    fd_set current_sockets;
    fd_set ready_sockets;
    int fd_max = newServ.getServer();

    FD_ZERO(&current_sockets);
    FD_SET(newServ.getServer(), &current_sockets);
    while (true)
    {
        ready_sockets = current_sockets;
        if ((select(fd_max + 1, &ready_sockets, nullptr, nullptr, nullptr)) < 0) {
            std::cerr << "Select errror: " << strerror(errno) << std::endl;
            exit(EXIT_FAILURE);
        }
        for (int i = 0; i < fd_max + 1; i++){
            if (FD_ISSET(i, &ready_sockets)){
                if (i == newServ.getServer()){
                    struct sockaddr_in addrClient;
                    socklen_t csize = sizeof(addrClient);
                    client = accept(newServ.getServer(), (struct sockaddr *) &addrClient, &csize);
                    if (fcntl(client, F_SETFL, O_NONBLOCK) < 0){
                        std::cout << "Fcntl error: " << strerror(errno) << std::endl;
                        exit(EXIT_FAILURE);
                    }
                    if (client < 0){
                        std::cerr << "Accept error: " << strerror(errno) << std::endl;
                        exit(EXIT_FAILURE);
                    }
                    if (client > fd_max){
                        fd_max = client;
                    }
                    FD_SET(client, &current_sockets);
                    if (max_sock < client) {
                        max_sock = client;
                    }
                }
                else{
                    char msg[1000];
                    memset(msg, 0x0, 1000);
                    size_t bytes_read;
                    bytes_read = recv(client, msg, 1000, 0);
                    int check = check_recv(msg);
                    std::cout << msg << std::endl;
                    if (bytes_read < 0){
                        std::cerr << "Recv error: " << strerror(errno) << std::endl;
                        exit(EXIT_FAILURE);
                    }
                    if (check == 1){
                        std::string toSend(newServ.get_rpl_msg("RPL_WELCOME", get_nickname(msg), get_username(msg)));
                        if (send(client, toSend.c_str(), toSend.size(), 0) < 0) {
                            std::cout << "Send error: " << strerror(errno) << std::endl;
                            exit(EXIT_FAILURE);
                        }
                    }
                    else if (check == 4){
                        std::string pong("PONG localhost");
                        if (send(client, pong.c_str(), pong.size(), 0) < 0){
                            std::cout << "Send error: " << strerror(errno) << std::endl;
                        }
                    }
                }
            }
        }
    }
    FD_CLR(FD_SETSIZE, &current_sockets);
	close(newServ.getServer());
	close(client);
    return 0;
}
	// int octet_recv = recv(newServ.getClient(), msg, 1000, 0);
    // std::string nickname = get_nickname(msg);
    // std::string username = get_username(msg);
    // std::string toSend(newServ.get_rpl_msg("RPL_WELCOME", get_nickname(msg), get_username(msg)));
    // if (send(newServ.getClient(), toSend.c_str(), toSend.size(), 0) < 0) {
    //     std::cerr << "Send error " << strerror(errno) << std::endl;
    //     exit(EXIT_FAILURE);
    // }

    // while (octet_recv > 0) {
    //     std::cout << msg << std::endl;
    //     if (octet_recv < 0){
    //         std::cerr << "Nothing to receive\n";
    //         break ;
    //     }
    //     if (!strncmp(msg, "PING localhost", strlen("PING localhost"))) {
    //         std::cout << "fefe" << std::endl;
    //         std::string pong("PONG localhost");
    //         send(newServ.getClient(), pong.c_str(), pong.size(), 0);
    //     }
    //     memset(msg, 0, 1000);
	//     octet_recv = recv(newServ.getClient(), msg, 1000, 0);
    // }