#include "../includes/Server.hpp"

int main(int ac, char **av) {

	if (ac != 3){
		std::cerr << "2 argument required : ./ircserv [ PORT ] [ PASSWORD ]\n";
		exit (1);
	}
	int					port = atoi(av[1]),
						client_len,
						kq,
						new_event,
						event_fd,
						connection_fd,
						cmd;
	std::string			toSend,
						password(av[2]),
						bufRecv;
	Server				srv(port, password);
	struct kevent		change_event[256],
						event[256];
	struct sockaddr_in	client_addr;
	char				buf[1024];
	size_t				bytes_read;

	client_len = sizeof(client_addr);
	memset(buf, 0, 1024);
	kq = kqueue();
	EV_SET(change_event, srv.getListen(), EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, 0);
	if (kevent(kq, change_event, 1, NULL, 0, NULL) < 0){
			perror("kevent");
			exit(EXIT_FAILURE);
	}

	while (true){
		if ((new_event = kevent(kq, NULL, 0, event, 1, NULL)) < 0){
			perror("kevent");
			exit(EXIT_FAILURE);
		}
		for (int i = 0; i < new_event; i++){
			event_fd = event[i].ident;
			if (event[i].flags & EV_EOF){
				std::cout << "Client has disconnect\n";
				close(event_fd);
			}
			else if (event_fd == srv.getListen()){
				if ((connection_fd = accept(event_fd, (struct sockaddr *)&client_addr, (socklen_t *)&client_len)) < 0){
					perror("kevent error");
				}
				EV_SET(change_event, connection_fd, EVFILT_READ, EV_ADD, 0 ,0, NULL);
				if (kevent(kq, change_event, 1, NULL, 0, NULL) < 0){
					perror("kevent error");
				}
			}
			else if (event[i].filter & EVFILT_READ){
				memset(buf, 0, 1024);
				toSend.clear();
				bufRecv.clear();
				bytes_read = recv(event_fd, buf, sizeof(buf), 0);
				std::cout << buf << std::endl;
				bufRecv = static_cast<std::string>(buf);
				cmd = srv.findCommand(bufRecv);
				if (cmd < 0)
					std::cout << "Command not found\n";
				else{
					toSend = srv.performCommand(cmd, buf);
					if (send(event_fd, toSend.c_str(), toSend.size(), 0) < 0){
						perror("Send error");
					}
				}
			}
		}
	}
	return (0);
}