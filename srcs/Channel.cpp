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
	cout << "SIZE :" << _user.size() << endl;
	cout << "TO SEND : " << toSend << endl;
	cout << "BUF : " << buf << endl;
}