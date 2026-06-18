#include "User.h"

User::User(const std::string& username, const std::string& password, const std::string& email)
{
	m_username = username;
	m_password = password;
	m_email = email;
}

User::User(const std::string& username, const std::string& password)
{
	m_username = username;
	m_password = password;
}

User::~User()
{
}

std::string User::getUsername() const
{
	return m_username;
}


std::string User::getPassword() const
{
	return m_password;
}


std::string User::getEmail() const
{
	return m_email;
}


void User::setUsername(const std::string& username)
{
	m_username = username;
}


void User::setPassword(const std::string& password)
{
	m_password = password;
}


void User::setEmail(const std::string& email)
{
	m_email = email;
}

std::ostream& operator<<(std::ostream& os, const User& user)
{
	os << "Username: " << user.m_username << "\n";
	os << "Password: " << user.m_password << "\n";
	os << "Email: " << user.m_email << std::endl;
	return os;
}

bool operator==(const User& lUser, const User& rUser)
{
	return lUser.m_username == rUser.m_username;
}