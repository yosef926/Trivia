#include "LoggedUser.h"


LoggedUser::LoggedUser(const std::string& username)
{
	m_username = username;
}

LoggedUser::LoggedUser(const LoggedUser& other)
    : m_username(other.m_username)
{
}

LoggedUser::LoggedUser()
	: m_username("")
{
}

LoggedUser::~LoggedUser()
{
}

std::string LoggedUser::getUsername() const
{
	return m_username;
}

bool LoggedUser::operator==(const LoggedUser& other) const
{
	return other.getUsername() == m_username;
}