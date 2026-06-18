#include "LoginManager.h"
#include "SqliteDataBase.h"

LoginManager::LoginManager(IDatabase* db)
	: m_database(db)
{
}

LoginManager::~LoginManager()
{
	delete m_database;
}


LoginStatus LoginManager::login(const std::string& username, const std::string& password)
{
	if (!m_database->doesUserExist(username) || !m_database->doesPasswordMatch(username, password))
	{
		std::cout << "Username or password are wrong" << std::endl;
		return LoginStatus::INVALID_CREDENTIALS;
	}

	LoggedUser newUser(username);
	auto it = std::find(m_loggedUsers.begin(), m_loggedUsers.end(), newUser);
	if (it == m_loggedUsers.end())
	{
		// Push the new user
		m_loggedUsers.push_back(newUser);

		std::cout << "[User *" << username << "* has logged in]" << std::endl;
		return LoginStatus::SUCCESS;
	}
	else return LoginStatus::USER_ALREADY_LOGGED;
}


SignupStatus LoginManager::signup(const std::string& username, const std::string& password, const std::string& email)
{
	if (m_database->doesUserExist(username))
	{
		std::cout << "Username already exist" << std::endl;
		return SignupStatus::USERNAME_ALREADY_EXIST;
	}
	m_database->addNewUser(username, password, email);
	// Push the new user
	LoggedUser newUser(username);
	m_loggedUsers.push_back(newUser);

	std::cout << "[User *" << username << "* has signed up]" << std::endl;
	return SignupStatus::SUCCESS;
}


LogoutStatus LoginManager::logout(const std::string& username)
{
	for (size_t i = 0; i < m_loggedUsers.size(); i++)
	{
		if (m_loggedUsers.at(i).getUsername() == username)
		{
			m_loggedUsers.erase(m_loggedUsers.begin() + i);
			std::cout << "[User *" << username << "* has logged out]" << std::endl;
			return LogoutStatus::SUCCESS;
		}
	}
	return LogoutStatus::FAIL;
}