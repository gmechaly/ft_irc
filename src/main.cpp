/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gmechaly <gmechaly@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/19 16:57:04 by gmechaly          #+#    #+#             */
/*   Updated: 2026/01/15 19:22:22 by gmechaly         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/irc_general.hpp"
#include "server/Server.hpp"

int main(int argc, char **argv) {
	if (argc != 3) {
		std::cerr << "Usage: ./ircserv <port> <password>" << std::endl;
		return 1;
	}
	int port = std::atoi(argv[1]);
    
	// ports below 1024 are reserved by the system (root)
    // max TCP is 65535
    if (port < 1024 || port > 65535) {
        std::cerr << "Error: Invalid port. Please use a port between 1024 and 65535." << std::endl;
        return 1;
    }
    std::string password = argv[2];
    try {
        Server server(port, password);
        server.init();
        std::cout << "Server initialized successfully." << std::endl;
        server.run();
    }
    catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    std::cout << "Server stopped." << std::endl;
    return 0;
}