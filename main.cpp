
#include "Server.hpp"
#include "Utils.hpp"
#include <string.h>
#include <string>

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
    return username;
}

int main(int ac, char **av) {

    if (ac != 2){
        std::cerr << "1 argument required : ./ircserv [ PORT ]\n";
        exit (1);
    }
    int port = atoi(av[1]);
	Server	newServ(port, "toto");
	char msg[1000];
    memset(msg, 0, 1000);
	int octet_recv = recv(newServ.getClient(), msg, 1000, 0);
    std::string nickname = get_nickname(msg);
    std::string username = get_username(msg);
    std::string toSend(newServ.get_rpl_msg("RPL_WELCOME", get_nickname(msg), get_username(msg)));
    if (send(newServ.getClient(), toSend.c_str(), toSend.size(), 0) < 0) {
        std::cerr << "Send error " << strerror(errno) << std::endl;
        exit(EXIT_FAILURE);
    }

    while (octet_recv > 0) {
        std::cout << msg << std::endl;
        if (octet_recv < 0){
            std::cerr << "Nothing to receive\n";
            break ;
        }
        if (!strncmp(msg, "PING localhost", strlen("PING localhost"))) {
            std::cout << "fefe" << std::endl;
            std::string pong("PONG localhost");
            send(newServ.getClient(), pong.c_str(), pong.size(), 0);
        }
        memset(msg, 0, 1000);
	    octet_recv = recv(newServ.getClient(), msg, 1000, 0);
    }
	close(newServ.getServer());
	close(newServ.getClient());
    return 0;
}