#pragma once

#include <string>

class LoggedUser
{
public:
	bool operator<(const LoggedUser& other) const {
		return this->m_username < other.m_username;
	}

	LoggedUser(const std::string& username);
	LoggedUser(const LoggedUser& other);
	LoggedUser();
	~LoggedUser();
	std::string getUsername() const;

	bool operator==(const LoggedUser& other) const;
private:
	std::string m_username;
};