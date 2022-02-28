#include "../includes/Channel.hpp"

int    Channel::findUser(int fd) const {
	std::vector<User>::const_iterator    it = _user.begin();
	std::vector<User>::const_iterator    ite = _user.end();
	int i = 0;

	while (it != ite){
		if ((*it).getFd() == fd){
			return (i);
		}
		it++;
		i++;
	}
	return (-1);
}

// void Channel::join_channel(int fd) {
// 	int i = findUser(fd);

// 	string nickname = _user[i].getNick();
// 	string username = _user[i].getUser();
// 	string hostname = _user[i].getHost();
// 	//string join(":" + nickname + "!" + username + "@" + hostname + " JOIN :#toto\r\n" + ":localhost 353 hkrifa = #toto :\r\n" + ":localhost 366 hkrifa #toto :End of NAMES list\r\n");
// 	string join(":" + nickname + "!" + username + "@" + hostname + " JOIN :#toto\r\n" + ":localhost 353 " + username + " = #" + _name + " :\r\n" + ":localhost 366 " + username + " #" + _name + " :End of NAMES list\r\n");
// 	string join2(":" + nickname + "!" + username + "@" + hostname + " JOIN #" + _name + "\r\n");
// 	cout << "ici\n";
// 	for(size_t k = 0; k <= _size; k++) {
// 		if (_size == 1) {
			
// 			send(fd, join.c_str(), join.size(), 0);
// 			cout << "---------------------- out ----------------------\n" << join;
// 		}
// 		else {
// 			if (k < _size) {
// 				int user_fd = _user[k].getFd();
// 				send(user_fd, join2.c_str(), join2.size(), 0);
// 				cout << "---------------------- out ----------------------\n" << join2;
// 			}
// 		}
// 	}
// 	if (_size != 0)
// 		send(fd, join.c_str(), join.size(), 0);
// }
int	Channel::findUser(string name) const{
	for (size_t i = 0; i < _user.size(); i++){
		if (name == _user[i].getNick())
			return (i);
	}
	return (-1);
}

void Channel::send_msg_to_channel(int fd, string buf) const {
	int i = findUser(fd);
	string toSend = ":" + _user[i].getNick() + "!" + _user[i].getUser() + "@" + _user[i].getHost() + " " + buf;
	if (_user.size() > 1) {
		for (size_t j = 0; j < _user.size(); j++) {
			int user_fd = _user[j].getFd();
			if (user_fd != fd) {
				send(user_fd, toSend.c_str(), toSend.size(), 0);
				cout << "---------------------- out ----------------------" << toSend << endl;
				
			}
		}
	}
}