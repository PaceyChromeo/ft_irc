#include "../includes/Server.hpp"

void Server::print_users() const { 
	std::vector<User>::const_iterator it = _user.begin();
	std::vector<User>::const_iterator ite = _user.end();
	int	i = 1;

	if (_user.empty())
		cout << "USER IS EMPTY\n";
	else{
		while (it != ite){
			cout << "********** USER n" << i << " ***********\n";
			cout << "_user->nickname: >>> " << (*it).getNick() << endl;
			cout << "_user->username: >>> " << (*it).getUser() << endl;
			cout << "_user->realname: >>> " << (*it).getReal() << endl;
			cout << "_user->host: >>> " << (*it).getHost() << endl;
			cout << "_user->mode: >>> " << (*it).getMode() << endl;
			cout << "_user->fd: >>> " << (*it).getFd() << endl;
			cout << "_user->nickname set : >>> " << ((*it).getConnectionFirst() == 1 ? TRUE : FALSE) << endl;
			cout << "_user->username set : >>> " << ((*it).getConnectionSecond() == 1 ? TRUE : FALSE) << endl;
			cout << "_user->registered : >>> " << ((*it).getConnectionThird() == 1 ? TRUE : FALSE) << endl;
			cout << "******************************\n\n";
			it++;
			i++;
		}
	}
}

string Server::get_err_msg(string error, string cmd, const User& user) const {
	if (error == "ERR_NICKNAMEGIVEN"){
		return (string(":localhost 431 :No nickname given\r\n"));
	}
	else if (error == "ERR_NICKNAMEINUSE"){
		return (string(":localhost 433 * " + user.getNick() + " :Nick already in use." + EOL));
	}
	else if (error == "ERR_NEEDMOREPARAMS"){
		return (string(":localhost 461 " + cmd + " :Not enough parameters" + EOL));
	}
	else if (error == "ERR_PASSWDMISMATCH"){
		return (string(":localhost 464 :Password incorrect\r\n"));
	}
	else
		return (0);
}

string Server::get_rpl_msg(string reply, const User& user) const {
	if (reply == "PING"){
		return (string(":localhost PONG :" + user.getHost() + EOL));
	}
	else if (reply == "PONG") {
		return (string(":localhost PONG :" + user.getHost() + EOL));
	}
	else if (reply == "WHOIS"){
		return (string(":localhost 311 " + user.getNick() + " " + user.getNick() + " " + user.getUser() + " " + "localhost * :" + user.getReal() + EOL));
	}
	else
		return (0);
}

string Server::get_nickname(string nick) const{
	size_t	pos = nick.find("NICK ") + 5;
	size_t	cr = nick.substr(pos, nick.size()).find("\r");
	string	nickname = nick.substr(pos, cr);

	return (nickname);
}

string Server::get_username(string user) const{
	size_t	pos = user.find("USER ") + 5;
	size_t	sp = user.substr(pos, user.size()).find(" ");
	string	username = user.substr(pos, sp);

	return (username);
}

string Server::get_realname(string real) const{
	size_t	colon = real.rfind(":") + 1;
	size_t	cr = real.substr(colon, real.size()).find("\r");
	string	realname = real.substr(colon, cr);

	return (realname);
}

string Server::get_passwd(string passwd) const{
	size_t	pos = passwd.find("PASS ") + 5;
	size_t	cr = passwd.substr(pos, passwd.size()).find("\r");
	string	pswd = passwd.substr(pos, cr);
	return (pswd);
}

string	Server::get_channel(string channel) const{
	size_t	pos = channel.find("#") + 1;
	size_t	cr = channel.substr(pos, channel.length()).find("\r");
	string	chan = channel.substr(pos, cr);
	return (chan);
}

int	Server::addNewUser(User& usr) {
	std::vector<User>::iterator it = _user.begin();
	std::vector<User>::iterator ite = _user.end();

	if (usr.getNick().empty())
		return (-1);
	while (it != ite){
		if ((*it).getNick() == usr.getNick()) {
			cout << "NICK ["<< usr.getNick() << "] already exists\n";
			return (0);
		}
		it++;
	}
	_user.push_back(usr);
	_size++;
	return (1);
}

int	Server::findUser(int fd) const {
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

int Server::findNick(string userhost) {
	for (size_t i = 0; i < _user.size(); i++) {
		if (userhost == _user[i].getNick()) 
			return (i);
	}
	return (-1);

}

int	Server::removeUser(int fd){
	std::vector<User>::const_iterator	it = _user.begin();
	std::vector<User>::const_iterator	ite = _user.end();
	int i = 0;

	while (it != ite){
		if ((*it).getFd() == fd){
			_user.erase(it);
			return (i);
		}
		it++;
		i++;
	}
	return (-1);
}

void	Server::createChannels() {
	_channel.push_back(Channel("lobby"));
	_channel.push_back(Channel("toto"));
	_channel.push_back(Channel("mago"));

	
}

int Server::findChannel(string name) {
	std::vector<Channel>::iterator it = _channel.begin();
	std::vector<Channel>::iterator ite = _channel.end();

	int i = 0;
	while (it != ite) {
		if ((*it).get_name() == name) {

			return (i);
		}
		it++;
		i++;
	}
	return (-1);
}

int	Server::findUserInChannel(int chan_index, string nick) const {
	for (size_t i = 0; i < _channel[chan_index].get_size(); i++){
		if (_channel[chan_index].get_user(i).getNick() == nick){
			return (i);
		}
	}
	return (-1);
}

int	Server::addNewChannel(string name, User &user) {
	if (findChannel(name) == -1) {
		Channel chan(name);
		User newuser(user);
		chan.set_user(newuser);
		_channel.push_back(chan);
		return (0);
	}
	return (-1);
}

int Server::addUserToChannel(string name, User &user) {
	int i = findChannel(name);

	if (i > -1){
		_channel[i].set_user(user);
		_channel[i].set_size(_channel[i].get_size() + 1);
		return (0);
	}
	return (-1);
}

int Server::removeUserFromChannel(string name, User& user){
	int i = findChannel(name);

	if (i > -1 && (_channel[i].get_size() > 0)) {
		int index = _channel[i].findUser(user.getNick());
		cout << "USER :" << user.getNick() << endl;
		cout << "INDEX :" << index << endl;
		vector<string>::const_iterator begin = _channel[i].get_nicks().begin();
		_channel[i].get_nicks().erase(begin + index);
		_channel[i].removeUser(user.getNick());
		_channel[i].set_size(_channel[i].get_size() - 1);
		return (0);
	}
	return (-1);
}

int	Server::findCommand(string buf) const {
	char		*args[16] = {	(char *)"PASS",
								(char *)"NICK",
								(char *)"USER",
								(char *)"MODE",
								(char *)"WHO",
								(char *)"KICK",
								(char *)"JOIN",
								(char *)"OPEN",
								(char *)"PART",
								(char *)"PING",
								(char *)"PONG",
								(char *)"PRIVMSG",
								(char *)"QUIT",
								(char *)"userhost",
								(char *)"TOPIC",
								(char *)"WHOIS"};
	int i = 0;
	
	while (i < 16){
		if (buf.find(args[i]) < BUF_SIZE)
			return (i);
		i++;
	}
	return (-1);
}

string	Server::performCommand(int cmd_nbr, string buf, int fd) {
	if (cmd_nbr == PASS){
		int	i = findUser(fd);

		if (i == -1)
			return EOL;
		else
			return (passCmd(this, buf, _user[i]));
	}

	else if (cmd_nbr == NICK){
		int	i = findUser(fd);

		if (i == -1)
			return EOL;
		else
			return(nickCmd(this, buf, _user[i]));
	}

	else if (cmd_nbr == USER){
		return (userCmd(this));
	}

	else if (cmd_nbr == MODE){
		int i = findUser(fd);
		if (i == -1)
			return EOL;
		else
			return (modeCmd(this, buf, _user[i], fd));
	}

	else if (cmd_nbr == WHO) {
		int i = findUser(fd);

		if (i == -1)
			return EOL;
		else
			return (whoCmd(this, _user[i]));
	}

	else if (cmd_nbr == KICK){
		int	i = findUser(fd);
	
		if (i == -1)
			return EOL;
		else
			return (kickCmd(this, _user[i]));
	}

	else if (cmd_nbr == JOIN) {
		string chan_name = buf.substr((buf.find("#") + 1), (buf.find("\n") - 2));
		chan_name.erase(chan_name.size() - 2);
		int i = findUser(fd);
		if (i == -1)
			return EOL;
		int j = findChannel(chan_name);
		string nickname = _user[i].getNick();
		string username = _user[i].getUser();
		string hostname = _user[i].getHost();
		string nicks;
		if (_channel[j].get_size() == 0) {
			_channel[j].set_nick("@" + nickname + " ");
		}
		else {
			_channel[j].set_nick(nickname + " ");
		}
		for (size_t i = 0; i <= _channel[j].get_size(); i++)
			nicks.append(_channel[j].get_nick(i));
		cout << "NICKS :" << nicks << endl;
		//string join(":" + nickname + "!" + username + "@" + hostname + " JOIN :#toto\r\n" + ":localhost 353 hkrifa = #toto :\r\n" + ":localhost 366 hkrifa #toto :End of NAMES list\r\n");
		string join(":" + nickname + "!" + username + "@" + hostname + " JOIN :#toto\r\n" + ":localhost 353 " + username + " = #" + chan_name + " :" + nicks + EOL + ":localhost 366 " + username + " #" + chan_name + " :End of NAMES list\r\n");
		string join2(":" + nickname + "!" + username + "@" + hostname + " JOIN #" + chan_name + "\r\n");
		for(size_t k = 0; k <= _channel[j].get_size(); k++) {
			if (_channel[j].get_size() == 0) {
				send(fd, join.c_str(), join.size(), 0);
				cout << "---------------------- out ----------------------\n" << join;
			}
			else {
				if (k < _channel[j].get_size()) {
					int user_fd = _channel[j].get_user()[k].getFd();
					send(user_fd, join2.c_str(), join2.size(), 0);
					cout << "---------------------- out ----------------------\n" << join2;
				}
			}
		}
		if (_channel[j].get_size() != 0)
			send(fd, join.c_str(), join.size(), 0);
		if (addNewChannel(chan_name, _user[i])) {
			addUserToChannel(chan_name, _user[i]);
		}
	}
	else if (cmd_nbr == OPEN){
		int	i = findUser(fd);
	
		if (i == -1)
			return EOL;
		else
			return (openCmd(this, _user[i]));
	}
	else if (cmd_nbr == PART){
		int	i = findUser(fd);

		if (i == -1)
			return EOL;
		else
			return (partCmd(this, buf, _user[i]));
	}
	else if (cmd_nbr == PING){
		int i = findUser(fd);

		if (i == -1)
			return(get_rpl_msg("PING", User()));
		else
			return(get_rpl_msg("PING", _user[i]));
	}
	else if (cmd_nbr == PONG){
		int i = findUser(fd);

		if (i == -1)
			return(get_rpl_msg("PONG", User()));
		else
			return(get_rpl_msg("PONG", _user[i]));

	}
	else if (cmd_nbr == PRIVMSG){
		int	i = findUser(fd);
	
		if (i == -1)
			return EOL;
		else
			return (privmsgCmd(this, buf, _user, fd));
	}
	else if (cmd_nbr == QUIT){
		int i = findUser(fd);

		if (i > -1){
			close(_user[i].getFd());
			removeUser(fd);
		}
		return EOL;
	}
	else if (cmd_nbr == userhost){
		int	i = findUser(fd);

		if (i == -1)
			return EOL;
		else
			return (userhostCmd(this, buf, _user[i]));
	}
	else if (cmd_nbr == TOPIC){
		int	i = findUser(fd);
	
		if (i == -1)
			return EOL;
		else
			return(topicCmd(this, _user[i]));
	}
	else if (cmd_nbr == WHOIS){
		int index = findUser(fd);

		if (index == -1)
			return(get_rpl_msg("WHOIS", User()));
		else
			return(get_rpl_msg("WHOIS", _user[index]));
	}
	return EOL;
}