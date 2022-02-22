#pragma once

#include "User.hpp"
#include "Channel.hpp"
#include <unistd.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/event.h>
#include <iostream>
#include <vector>

#define	KICK_TIME 300

enum e_cmd {	KICK,
				JOIN,
				ME,
				MODE,
				NICK,
				OPEN,
				PART,
				PASS,
				PING,
				PRIVMSG,
				QUIT,
				USER,
				TOPIC,
				WHOIS };

using namespace std;

class Server{

	public:
		Server(int port, std::string pswd) : _port(port), _enable(1), _password(pswd){

			if ((_listen_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
				perror("Error opening socket");
				exit(EXIT_FAILURE);
			}
			setsockopt(_listen_fd, SOL_SOCKET, SO_REUSEADDR, &_enable, sizeof(int));
			memset(&_serv_addr, 0, sizeof(_serv_addr));
			_serv_addr.sin_family = AF_INET;
    		_serv_addr.sin_addr.s_addr = INADDR_ANY;
    		_serv_addr.sin_port = htons(_port);

    		if ((bind(_listen_fd, (const struct sockaddr *)&_serv_addr, sizeof(_serv_addr))) < 0){
				perror("Error binding socket");
				exit(EXIT_FAILURE);
			}

    		if ((listen(_listen_fd, 30)) < 0){
				perror("Listen error");
				exit(EXIT_FAILURE);
			}
			
		};

		std::string get_rpl_msg(std::string protocol, const User& user) const {
			if (protocol == "RPL_WELCOME"){
				return (std::string(":localhost 001 " + user.getNick() + "\n\"Welcome to the Internet Relay Chat Network\"\n" + user.getNick() + "!" + user.getUser() + "@" + user.getHost() + "\"\r\n"));
			}
			else if (protocol == "PING"){
				return (std::string(":localhost PONG " + user.getHost() + "\r\n"));
			}
			else if (protocol == "ERR_NICKNAMEINUSE"){
				return (std::string(":localhost 433 *\n" + user.getNick() + " : Nick already in use.\r\n"));
			}
			else if (protocol == "WHOIS"){
				return (std::string(":localhost 311 " + user.getNick() + "\n" + user.getNick() + " " + user.getUser() + " " + "localhost * :" + user.getReal() + "\r\n"));
			}
			else
				return (0);
		}

		std::string get_nickname(std::string nick) const{
			int			find = nick.find("NICK ");
			int			find_endl = nick.substr(find + 5, nick.size()).find("\n");
			std::string	nickname = nick.substr(find + 5, find_endl);			
			nickname.erase(nickname.size() - 1);
			return (nickname);
		}

		std::string get_username(std::string user) const{
			int			find = user.find("USER ");
			int			find_space = user.substr(find + 5, user.size()).find(" ");
			std::string	username = user.substr(find + 5, find_space);
			return (username);
		}

		std::string get_realname(std::string real) const{
			int			find = real.rfind(":");
			int			find_endl = real.substr(find + 1, real.size()).find("\n");
			std::string	realname = real.substr(find + 1, find_endl);
			realname.erase(realname.size() - 1);
			return (realname);
		}

		void print_users() const { 
			std::vector<User>::const_iterator it = _user.begin();
			std::vector<User>::const_iterator ite = _user.end();
			int	i = 1;
			while (it != ite){
				std::cout << "********** USER n" << i << " ***********\n";
				std::cout << "_user->nickname: >>> " << (*it).getNick() << std::endl;
				std::cout << "_user->username: >>> " << (*it).getUser() << std::endl;
				std::cout << "_user->host: >>> " << (*it).getHost() << std::endl;
				std::cout << "_user->mode: >>> " << (*it).getMode() << std::endl;
				std::cout << "_user->fd: >>> " << (*it).getFd() << std::endl;
				std::cout << "******************************\n\n";
				it++;
				i++;
			}
		}
		
		int	addNewUser(User& usr) {
			std::vector<User>::iterator it = _user.begin();
			std::vector<User>::iterator ite = _user.end();

			while (it != ite){
				if ((*it).getNick() == usr.getNick()) {
					std::cout << "NICK ["<< usr.getNick() << "] already exists\n";
					return (0);
				}
				it++;
			}
			_user.push_back(usr);
			return (1);
		}

		void	createChannels() {
			_channel.push_back(Channel("lobby"));
			_channel.push_back(Channel("toto"));
			_channel.push_back(Channel("mago"));

			
		}

		int    findUser(int fd) const {
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

		int	removeUser(int fd){
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

		int findChannel(std::string name) {

			std::vector<Channel>::iterator it = _channel.begin();
			std::vector<Channel>::iterator ite = _channel.end();

			int i = 0;
			while (it != ite) {
				if ((*it).get_channel_name() == name) {
					std::cout << "CHANNEL [" << name << "] already exists\n";
					return (i);
				}
				it++;
				i++;
			}
			return (-1);
		}

		int	addNewChannel(std::string name, User &user) {

			if (findChannel(name) == -1) {
				Channel chan(name);
				User newuser(user);
				chan.set_user(newuser);
				_channel.push_back(chan);
				return 0;
			}
			return 1;
		}

		int addUserToChannel(std::string name, User &user) {

			int i = findChannel(name);
			if (i > 0)
				_channel[i].set_user(user);
			return 0;
		}

		int	findCommand(std::string buf) const {
			char		*args[14] = {	(char *)"KICK",
										(char *)"JOIN",
										(char *)"ME",
										(char *)"MODE",
										(char *)"NICK",
										(char *)"OPEN",
										(char *)"PART",
										(char *)"PASS",
										(char *)"PING",
										(char *)"PRIVMSG",
										(char *)"QUIT",
										(char *)"USER",
										(char *)"TOPIC",
										(char *)"WHOIS"};
			std::string	cmd_name;
			int i = 0;
			while (i < 14){
				cmd_name = args[i];
				if (buf.find(cmd_name) < 1024)
					return (i);
				i++;
			}
			return (-1);
		}

		std::string	performCommand(int cmd_nbr, std::string buf, int connection_fd, int event_fd) {
			std::string toSend("");

			if ((buf.find("NICK")) < 1024 && (buf.find("USER")) < 1024){
				User		newUser(get_username(buf), get_nickname(buf), get_realname(buf), "localhost", "invisible", connection_fd);
				if (addNewUser(newUser))
					toSend = get_rpl_msg("RPL_WELCOME", newUser);
				else{
					toSend = get_rpl_msg("ERR_NICKNAMEINUSE", newUser);
					removeUser(connection_fd);
					close(connection_fd);
				}
				print_users();
			}
			else if (cmd_nbr == KICK){

			}
			else if (cmd_nbr == JOIN) {
				std::cout << "buf:" << buf << std::endl;
				// int start = buf.find("#") + 1;
				// int endl = buf.find("\n") - 2;
				std::string chan_name = buf.substr((buf.find("#") + 1), (buf.find("\n") - 2));
				chan_name.erase(chan_name.size() - 2);
				std::cout << chan_name << std::endl;
				int fd = findUser(event_fd);
				if (addNewChannel(chan_name, _user[fd])) {
					addUserToChannel(chan_name, _user[fd]);
				}
			}
			else if (cmd_nbr == ME){
				
			}
			else if (cmd_nbr == MODE){
				
			}
			else if (cmd_nbr == NICK){

			}
			else if (cmd_nbr == OPEN){
				
			}
			else if (cmd_nbr == PART){
				
			}
			else if (cmd_nbr == PASS){
				
			}
			else if (cmd_nbr == PING){
				toSend = get_rpl_msg("PING", _user[findUser(connection_fd)]);
			}
			else if (cmd_nbr == PRIVMSG){

			}
			else if (cmd_nbr == QUIT){
				int fd = findUser(connection_fd);
				close(_user[fd].getFd());
				removeUser(connection_fd);
			}
			else if (cmd_nbr == USER){
				
			}
			else if (cmd_nbr == TOPIC){
				
			}
			else if (cmd_nbr == WHOIS){
				toSend = get_rpl_msg("WHOIS", _user[findUser(connection_fd)]);
			}
			return (toSend);
		}

		~Server() {};

		int					getListen() const { return this->_listen_fd; };
		std::vector<User>	getUser() const { return this->_user; };

	private:
		int						_listen_fd;
		int						_port;
		int						_enable;
		std::string				_password;
		std::vector<User>		_user;
		std::vector<Channel>	_channel;
		struct sockaddr_in		_serv_addr;
		
		Server() {};
		Server(const Server& cpy) { (void)cpy; };
		Server& operator=(const Server & rhs) { (void)rhs; return *this; };
};