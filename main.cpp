
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
    char *tmp = ft_substr(tab[6], 1, (strlen(tab[6]) - 2));
    std::string username(tmp);
    username.erase(username.size() - 1);
    free_tab(tab);
    return username;
}

char *get_hostname(char *msg) {

        char **tab = ft_split(msg, ' ');
        char *hostname = tab[5];
        free_tab(tab);
        return hostname;
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
    while (octet_recv > 0) {
        std::cout << msg << std::endl;
        if (octet_recv < 0){
            std::cerr << "Nothing to receive\n";
            close(newServ.getServer());
            close(newServ.getClient());
            exit(EXIT_FAILURE);
        }
        std::string nickname = get_nickname(msg);
        std::string username = get_username(msg);
        std::string toSend("001 " + nickname + "\n\"Welcome to the Internet Relay Chat Network\"\n" + nickname + "!" + username + "@" + "localhost" + "\"");
        sleep(2);
        if (send(newServ.getClient(), toSend.c_str(), toSend.size(), 0) < 0) {
            std::cout << "Send error " << strerror(errno) << std::endl;
            exit(EXIT_FAILURE);
        }
        memset(msg, 0, 1000);
	    octet_recv = recv(newServ.getClient(), msg, 1000, 0);
    }
	close(newServ.getServer());
	close(newServ.getClient());
    return 0;
}