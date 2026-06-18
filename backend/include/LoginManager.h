#pragma once

#include <vector>

#include "IDatabase.h"
#include "LoggedUser.h"
#include "IRequestHandler.h"

class LoginManager
{
public:
	LoginManager(IDatabase* db);
	~LoginManager();

	LoginStatus login(const std::string& username, const std::string& password);
	SignupStatus signup(const std::string& username, const std::string& password, const std::string& email);
	LogoutStatus logout(const std::string& username);

private:
	IDatabase* m_database;
	std::vector<LoggedUser> m_loggedUsers;
};