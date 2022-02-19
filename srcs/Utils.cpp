#include "../includes/Utils.hpp"
#include "../includes/Server.hpp"
#include "../includes/User.hpp"

std::string	parsing_buf(std::string buf, Server& srv){
	std::string toSend;

	if ((buf.find("NICK") < 1024) && (buf.find("USER")) < 1024){
		User newUser(srv.get_username(buf), srv.get_nickname(buf), "localhost", "invisible");
		srv.addNewUser(newUser);
		toSend = srv.get_rpl_msg("RPL_WELCOME", buf);
	}
	else if ((buf.find("PING ")) < 1024){
		toSend = srv.get_rpl_msg("PING", buf);
	}
	return (toSend);
}