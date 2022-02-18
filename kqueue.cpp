#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/event.h>
#include <string.h>
#include <unistd.h>
#include <iostream>

int main(){ 
	int					listen_fd;
	int					connection_fd;
	int					port = 6667;
	int					client_len;
	int					kq;
	int					new_event;
	struct kevent		change_event[32];
	struct kevent		event[32];
	struct sockaddr_in	serv_addr;
	struct sockaddr_in	client_addr;

	if ((listen_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
		perror("Error opening socket");
		exit(EXIT_FAILURE);
	}

	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(port);

	if (bind(listen_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0){
		perror("Error binding socket");
		exit(EXIT_FAILURE);
	}

	listen(listen_fd, 30);
	client_len = sizeof(client_addr);
	kq = kqueue();
	// Create event 'filter', these are the events we want to monitor.
    // Here we want to monitor: socket_listen_fd, for the events: EVFILT_READ 
    // (when there is data to be read on the socket), and perform the following
    // actions on this kevent: EV_ADD and EV_ENABLE (add the event to the kqueue 
    // and enable it).
	EV_SET(change_event, listen_fd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, 0);

	if (kevent(kq, change_event, 1, NULL, 0, NULL) == -1){
			perror("kevent");
			exit(EXIT_FAILURE);
	}

	while(true){
		// Check for new events, but do not register new events with
        // the kqueue. Hence the 2nd and 3rd arguments are NULL, 0.
        // Only handle 1 new event per iteration in the loop; 5th
        // argument is 1.
		if ((new_event = kevent(kq, NULL, 0, event, 1, NULL)) < 0){
			perror("kevent");
			exit(EXIT_FAILURE);
		}
		
		for (int i = 0; i < new_event; i++){
			int event_fd = event[i].ident;
			// When the client disconnects an EOF is sent. By closing the file
			// descriptor the event is automatically removed from the kqueue.
			if (event[i].flags & EV_EOF)
			{
				std::cout << "Client has disconnected\n";
				close(event_fd);
			}
			// If the new event's file descriptor is the same as the listening
            // socket's file descriptor, we are sure that a new client wants 
            // to connect to our socket.
			else if (event_fd == listen_fd){
				// Incoming socket connection on the listening socket.
                // Create a new socket for the actual connection to client.
				if ((connection_fd = accept(event_fd, (struct sockaddr *)&client_addr, (socklen_t *)&client_len)) < 0){
					perror("Accept socket error");
				}
				// Put this new socket connection also as a 'filter' event
                // to watch in kqueue, so we can now watch for events on this
                // new socket.
				EV_SET(change_event, connection_fd, EVFILT_READ, EV_ADD, 0,0, NULL);
				if (kevent(kq, change_event, 1, NULL, 0, NULL) < 0){
					perror("kevent error");
				}
			}
			else if (event[i].filter & EVFILT_READ){
				char buf[1024];
				size_t bytes_read = recv(event_fd, buf, sizeof(buf), 0);
				std::cout << "Read: " << buf << " of " << bytes_read << " bytes.\n";
				if (!strncmp(buf, "CAP LS", 6)){
                        std::string toSend = "001 pjacob\n\"Welcome to the Internet Relay Network\"\npjacob!pjacob@localhost\"";
                        if (send(event_fd, toSend.c_str(), toSend.size(), 0) < 0) {
                            std::cout << "Send error: " << strerror(errno) << std::endl;
                            exit(EXIT_FAILURE);
                        }
                    }
				else if (!strncmp(buf, "PING ", 5)){
					std::string pong("PONG localhost");
					if (send(event_fd, pong.c_str(), pong.size(), 0) < 0){
						std::cout << "Send error: " << strerror(errno) << std::endl;
					}
				}
			}
		}
	}
	return (0);
}
