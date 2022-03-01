#include "../includes/Command.hpp"

string passCmd(Server* srv, string buf, User& user){
	string	pswd = srv->get_passwd(buf);

	if (pswd != srv->getPassword()){
		return (srv->get_err_msg("ERR_PASSWDMISMATCH", "", user));
	}
	else{
		srv->setPassEnable(1);
		return ("");
	}
}

string	nickCmd(Server* srv, string buf, User& user){
	string	newNick = srv->get_nickname(buf);
	string	toSend;

	if (newNick == user.getNick())
		return (EOL);
	else{
		user.setNick(newNick);
		toSend = "You're know known as " + newNick + EOL;
		return toSend;
	}
}

string	userCmd(Server* srv){
	(void)srv;
	return ("USER CMD\r\n");
}

string	modeCmd(Server* srv, string buf, User& user, int fd){
	string	toSend;
	string	tmp;

	if (buf.find("WHOIS") < BUF_SIZE){
		tmp = srv->get_rpl_msg("WHOIS", user);
		if (send(fd, tmp.c_str(), tmp.size(), 0) < 0){
			perror("Send error");
		}
	}
	if (buf.find("#") < BUF_SIZE){
		if (buf.rfind(" b") < BUF_SIZE) {
			toSend = ":localhost 368 " + user.getNick() + " #toto :End of channel ban list" + EOL;
		}
		else
			toSend = ":localhost 324 " + user.getNick() + " #toto" + EOL;
	}
	else{
		toSend = ":" + user.getNick() + "!" + user.getUser() + "@" + user.getHost() + " MODE " + user.getNick() + " :+i" + EOL;
	}
	return (toSend);
}

string whoCmd(Server* srv, User& user){
	string	nickname = user.getNick();
	string	toSend;

	(void)srv;
	toSend = ":localhost 352 " +  nickname + " #toto " + nickname + " localhost " + nickname + " H*@ :0 " + nickname + EOL + ":localhost 315 " + nickname + " #toto :End of /WHO list." + EOL;
	return (toSend);
}

string kickCmd(Server* srv, User& user){
	(void)srv;
	(void)user;
	return ("KICK CMD\r\n");
}

string openCmd(Server* srv, User& user){
	(void)srv;
	(void)user;
	return ("OPEN CMD\r\n");
}

string	partCmd(Server* srv, string buf, User& usr){
	string	nick = usr.getNick();
	string	user = usr.getUser();
	string	host = usr.getHost();
	string	chan = srv->get_channel(buf);
	string	toSend;

	srv->removeUserFromChannel(chan, usr);
	toSend = ":" + nick + "!" + user + "@" + host + " " + buf + EOL;
	return (toSend);
}

string	privmsgCmd(Server* srv, string buf, vector<User>& usr, int fd){
	if(buf.find("#") < BUF_SIZE){
		string chan_name = buf.substr((buf.find("#") + 1), (buf.find(" ") - 1));
		chan_name.erase(chan_name.size() - 2);
		int j = srv->findChannel(chan_name);
		srv->getChannel(j).send_msg_to_channel(fd, buf);
	}
	else{
		int whiteSpace = buf.find(" ");
		int colon = buf.find(":");
		int index = srv->findUser(fd);
		if (index == -1)
			return EOL;
		string user = buf.substr(whiteSpace + 1, (colon - whiteSpace) - 2);
		string mmm = buf.substr(colon + 1, buf.length() - (colon + 3)) + EOL;
		string msg = ":" + usr[index].getNick() + "!" + usr[index].getUser() + "@localhost " + buf + EOL;
		int userIndex = srv->findNick(user);
		if (userIndex < 0)
			return EOL;
		else {
			send(usr[userIndex].getFd(), msg.c_str(), msg.length(), 0);
		}
	}
	return EOL;
}

string	userhostCmd(Server* srv, string buf, User& user){
	string toSend;	

	(void)srv;
	if (user.getConnectionThird()){
		size_t	pos = buf.find(" ");
		size_t	cr = buf.substr(pos + 1, buf.size()).find("\r");
		string	userhost = buf.substr(pos + 1, cr);
		if (userhost == user.getNick())
			return (EOL);
		else{
			toSend = userhost + EOL;
			return (toSend);
		}
	}
	return EOL;
}

string topicCmd(Server* srv, User& user){
	(void)srv;
	(void)user;
	return ("TOPIC CMD\r\n");
}