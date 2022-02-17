
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
    while (true) {
		int octet_recv = recv(newServ.getClient(), msg, 1000, 0);
        std::cout << msg;
        if (octet_recv < 0){
            std::cerr << "Nothing to receive\n";
            close(newServ.getServer());
            close(newServ.getClient());
            exit(1);
        }
        std::string nickname(get_nickname(msg));
        std::string username(get_username(msg));
        std::string toSend("001 " + nickname + "\n\"Welcome to the Internet Relay Chat Network\"\n" + nickname + "!" + username + "@" + "localhost" + "\"");
        send(newServ.getClient(), &toSend, toSend.length(), 0);
        memset(msg, 0, 1000);
        //User User(get_username(msg), get_nickname(msg), get_hostname(msg), false);
        //std::string toSend("001 " + User.getNick() + "\n\"Welcome to the Internet Relay Chat Network\"\n" + User.getNick() + "!" + User.getUser() + "@" + "localhost" + "\"");
        //std::cout << toSend << std::endl;
        //send(newServ.getClient(), &toSend, toSend.size(), 0);
    }
	close(newServ.getServer());
	close(newServ.getClient());
    return 0;
}