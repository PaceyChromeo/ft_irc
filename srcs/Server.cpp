#include "../includes/Server.hpp"

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
	if (reply == "RPL_WELCOME"){
		return (string(":localhost 001 " + user.getNick() + " Welcome to the Internet Relay Chat Network " + user.getNick() + "!" + user.getUser() + "@" + user.getHost() + EOL));
	}
	else if (reply == "PING"){
		return (string(":localhost PONG " + user.getHost() + EOL));
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
			return 1;
	}
	return 0;

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
		if ((*it).get_channel_name() == name) {
			cout << "CHANNEL [" << name << "] already exists\n";
			return (i);
		}
		it++;
		i++;
	}
	return (-1);
}

int	Server::addNewChannel(string name, User &user) {

	if (findChannel(name) == -1) {
		Channel chan(name);
		User newuser(user);
		chan.set_user(newuser);
		_channel.push_back(chan);
		return 0;
	}
	return 1;
}

int Server::addUserToChannel(string name, User &user) {

	int i = findChannel(name);
	if (i > 0)
		_channel[i].set_user(user);
	return 0;
}

int	Server::findCommand(string buf) const {
	char		*args[13] = {	(char *)"PASS",
								(char *)"NICK",
								(char *)"USER",
								(char *)"MODE",
								(char *)"KICK",
								(char *)"JOIN",
								(char *)"OPEN",
								(char *)"PART",
								(char *)"PING",
								(char *)"PRIVMSG",
								(char *)"QUIT",
								(char *)"userhost",
								(char *)"TOPIC"};
	int i = 0;
	
	while (i < 13){
		if (buf.find(args[i]) < BUF_SIZE)
			return (i);
		i++;
	}
	return (-1);
}

string	Server::performCommand(int cmd_nbr, string buf, int connection_fd, int event_fd) {
	string toSend("");

	if (cmd_nbr == PASS){
		string	pswd = get_passwd(buf);

		if (pswd != getPassword()){
			toSend = get_err_msg("ERR_PASSWDMISMATCH", "", _user[findUser(event_fd)]);
		}
		else{
			_passEnable = 1;
		}
	}
	else if (cmd_nbr == NICK){
		int		fd = findUser(event_fd);
		string	newNick = get_nickname(buf);
		
		if (fd == -1){
			User	newUser(newNick, "", "", "localhost", "invisible", connection_fd, 1, 0, 0);
			addNewUser(newUser);
		}
		else if (_user[fd].getConnectionSecond() && !_user[fd].getConnectionFirst()){
			for (size_t i = 0; i < _user.size(); i++){
				if (_user[i].getNick() == newNick){
					toSend = get_err_msg("ERR_NICKNAMEINUSE", "", _user[i]);
					return (toSend);
				}
			}
			_user[fd].setNick(newNick);
			_user[fd].setConnectionFirst(1);
			_user[fd].setConnectionThird(1);
			if (_passEnable)
				toSend = get_rpl_msg("RPL_WELCOME", _user[fd]);
		}
		else if (_user[fd].getConnectionThird()){
			size_t	pos = buf.find(" ");
			size_t	cr = buf.substr(pos + 1, buf.size()).find("\r");
			string	userhost = buf.substr(pos + 1, cr);
			
			if (userhost == _user[fd].getNick())
				toSend = EOL;
			else{
				_user[fd].setNick(newNick);
				toSend = "You're know known as" + newNick + EOL;
			}
		}
		// if (buf.find("USER") < BUF_SIZE){
		// 	int	user_added = addNewUser(newUser);
		// 	if (user_added == -1)
		// 		toSend = get_err_msg("ERR_NICKNAMEGIVEN", "", newUser);
		// 	else if (user_added == 0)
		// 		toSend = get_err_msg("ERR_NICKNAMEINUSE", "", newUser);
		// 	else
		// 		toSend = get_rpl_msg("RPL_WELCOME", newUser);
		// }
		// else if (_user[fd].getNick() == newNick)
		// 	return ("");
		// else{
		// 	_user[fd].setNick(newNick);
		// 	toSend = "You're now known as " + newNick + EOL;
		// }
	}
	else if (cmd_nbr == USER){
		int		fd = findUser(event_fd);
		string	newUserName = get_username(buf);
		string	newReal = get_realname(buf);

		if (fd == -1){
			User	newUser("", newUserName, newReal, "localhost", "invisible", connection_fd, 0, 1, 0);
			addNewUser(newUser);
		}
		else if (_user[fd].getConnectionFirst() && !_user[fd].getConnectionSecond()){
			_user[fd].setUser(newUserName);
			_user[fd].setReal(newReal);
			_user[fd].setConnectionSecond(1);
			_user[fd].setConnectionThird(1);
			if (_passEnable)
				toSend = get_rpl_msg("RPL_WELCOME", _user[fd]);
		}
	}
	else if (cmd_nbr == MODE){
		
	}
	else if (cmd_nbr == KICK){

	}
	else if (cmd_nbr == JOIN) {
		cout << "buf: " << buf << endl;
		// int start = buf.find("#") + 1;
		// int endl = buf.find("\n") - 2;
		string chan_name = buf.substr((buf.find("#") + 1), (buf.find("\n") - 2));
		chan_name.erase(chan_name.size() - 2);
		cout << chan_name << endl;
		int fd = findUser(event_fd);
		if (addNewChannel(chan_name, _user[fd])) {
			addUserToChannel(chan_name, _user[fd]);
		}
	}
	else if (cmd_nbr == OPEN){
		
	}
	else if (cmd_nbr == PART){
		
	}
	else if (cmd_nbr == PING){
		toSend = get_rpl_msg("PING", _user[findUser(event_fd)]);
	}
	else if (cmd_nbr == PRIVMSG){

	}
	else if (cmd_nbr == QUIT){
		int fd = findUser(event_fd);

		close(_user[fd].getFd());
		removeUser(event_fd);
	}
	else if (cmd_nbr == userhost){
		int	fd = findUser(event_fd);

		if (_user[fd].getConnectionThird()){
			size_t	pos = buf.find(" ");
			size_t	cr = buf.substr(pos + 1, buf.size()).find("\r");
			string	userhost = buf.substr(pos + 1, cr);
			if (userhost == _user[fd].getNick())
				toSend = EOL;
			else
				toSend = userhost + EOL; 
		}
	}
	else if (cmd_nbr == TOPIC){
	}
	return (toSend);
}