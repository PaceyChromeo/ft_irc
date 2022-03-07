#pragma once

#include <iostream>
#include "Server.hpp"

using namespace std;

class Server;
class User;

string	passCmd(Server* srv, string buf, User& user);
string	nickCmd(Server* srv, string buf, User& user);
string	userCmd(Server* srv, string buf, User& user);
void	actualizeMode(string mode, User& user);
string	modeCmd(Server* srv, string buf, User& user, int fd);
string	whoCmd(Server* srv, User& user);
string	kickCmd(Server* srv, User& user, string buf);
string	partCmd(Server* srv, string buf, User& user);
string	privmsgCmd(Server* srv, string buf, vector<User>& usr, int fd);
string	noticeCmd(Server* srv, string buf, vector<User>& usr, int fd);
string	quitCmd(string buf, User& usr);
string	userhostCmd(Server* srv, string buf, User& user);
void    joinCmd(Server *srv, User &user, int fd, Channel& channel);
string&	eraseCrAndNl(string& buf);