
#include "Server.hpp"

std::string get_nickname(char *msg) {

    std::string str(msg);
    int  find = str.find("K ");
    int find_endl = str.substr(find+2, str.size()).find("\n");
    std::string nickname = str.substr(find+2, find_endl);

    return nickname;
}

std::string get_username(char *msg) {

    std::string str(msg);
    int index = str.rfind(":");
    int len = str.length();
    std::string username = str.substr(index + 1, len);

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
	ssize_t octet_recv = recv(newServ.getClient(), msg, 1000, 0);
	if (octet_recv < 0){
		std::cerr << "Nothing to receive\n";
		close(newServ.getServer());
		close(newServ.getClient());
		exit(1);
	}
    while (octet_recv > 0) {
        std::cout << "Message recu : " << msg;
		octet_recv = recv(newServ.getClient(), msg, 1000, 0);
        User User(get_username(msg), get_nickname(msg), false);
    }
	close(newServ.getServer());
	close(newServ.getClient());
    return 0;
}