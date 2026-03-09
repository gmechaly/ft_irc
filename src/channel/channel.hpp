#ifndef CHANNEL_HPP
# define CHANNEL_HPP

#include "../../includes/irc_general.hpp"
#include "../client/Client.hpp"

typedef struct	s_options
{
	bool		i; //true for invite only
	bool		t; //true for topic protection (can only be modified by operators)
	bool		k; //true means password protected channel
	std::string	pwd; //relevant only if k is true
	bool		l; //true means there is a user limit
	int			limit; //maximum users on the channel
}				t_opt;


class Channel {
	private:
		std::string				_name;
		std::vector<Client*>	_members;
		std::vector<Client*>	_operators;
		std::string				_topic;
		t_opt					_options;
		std::set<std::string>	_invitedUsers;

	public:
		Channel();
		Channel(const Channel &src);
		Channel &operator=(const Channel &src);
		Channel(std::string name);
		~Channel();

		void	init_opt();
		void	set_opt(char opt, int sign, std::string arg);
		t_opt	getOptions();

		std::string getName() const;
		void broadcast(std::string message);
		void broadcast(std::string message, Client* exclude);
		bool isMember(Client *c) const;
		void addMember(Client *c);
		void removeMember(Client *c);
		void addOperator(Client *c);
		void removeOperator(Client *c);
		void setTopic(std::string t);
		void addInvite(Client *c);
		std::string getTopic() const;
		std::vector<Client*> getMembers() const;
		std::set<std::string> getInvitedUsers() const;
		Client* getMember(int fd);
		Client*	getOperator(int fd);
};

#endif