/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gmechaly <gmechaly@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/19 16:56:54 by gmechaly          #+#    #+#             */
/*   Updated: 2026/02/03 17:12:49 by gmechaly         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_H
#define SERVER_H

#include "../../includes/irc_general.hpp"
#include "../client/Client.hpp"
#include "../channel/channel.hpp"
#include "../commands/commands.hpp"

class Server {
	public:
		Server();
		Server(int port, const std::string &pwd);
		Server(const Server &src);
		Server &operator=(const Server &src);
		~Server();

		void	run();
		void	init();

		Client	*getClientByFd(int fd);
		Client	*getClientByNick(std::string nick);
		Channel	*getChannelByName(std::string name);
		Channel	*createChannel(std::string name);

		int								getPort() const;
		int								getFdServ() const;
		std::vector<struct pollfd>		getPollfds() const;
		std::string						getPwd() const;
		std::map<int, Client*>			getClients() const;
		std::map<std::string, Channel*>	getChannels() const;
		std::string						getName() const;

	private:
		std::string						_name;
		int								_port;
		int								_fdserv;
		std::vector<struct pollfd>		_pollfds;
		std::string						_pwd;
		std::map<int, Client*>			_client;
		std::map<std::string, Channel*>	_channels;

		void	setupSocket();
		int		setNonBlocking(int fd);
		void	initPoll();
		void	acceptNewClient();
		void	handleCLientRead(int fd);
		void	handleClientDisconnect(int fd, Command *cmd);
		void	cleanup();
};

#endif