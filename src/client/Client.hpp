#ifndef CLIENT_HPP
# define CLIENT_HPP

#include "../../includes/irc_general.hpp"

class Server;

class Client {
    private:
		int	_fd;
		std::string	_nickname;
		std::string	_username;
		std::string	_buffer;
		std::string _ip;
		bool	_isRegistered;
		bool	_hasPass;

	public:
		Client();
		Client(int fd, std::string ip);
		Client(const Client &src);
		Client &operator=(const Client &src);
		~Client();

		int	getFd() const;
		std::string	getNickname() const;
		std::string	getUsername() const;
		std::string	&getBuffer();
		std::string getIp() const;
		bool		getIsRegistered() const;
		bool		getHasPass() const;

		void	setNick(std::string n);
		void	setUser(std::string u);
		void	setBuffer(std::string b);
		void	setIp(std::string ip);
		void	setIsRegistered(bool r);
		void	setHasPass(bool p);

		void reply(std::string msg, Server *srv);
		void broadcast_reply(std::string msg);
};

#endif
