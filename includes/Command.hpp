#pragma once

#include <iostream>
#include "Server.hpp"

using namespace std;

class Server;
class User;

string	passCmd(Server* srv, string buf, User& user);
string	nickCmd(Server* srv, string buf, User& user);
string	userCmd(Server* srv);
string	modeCmd(Server* srv, string buf, User& user, int fd);
string	whoCmd(Server* srv, User& user);
string	kickCmd(Server* srv, User& user);
string	openCmd(Server* srv, User& user);
string	partCmd(Server* srv, string buf, User& user);
string	privmsgCmd(Server* srv, string buf, vector<User>& usr, int fd);
string	userhostCmd(Server* srv, string buf, User& user);
string	topicCmd(Server* srv, User& usr);