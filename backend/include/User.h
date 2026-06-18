#pragma once

#include <iostream>
#include <string>


class User
{
public:
	User(const std::string& username, const std::string& password, const std::string& email);
	User(const std::string& username, const std::string& password);
	~User();

	// Getters
	std::string getUsername() const;
	std::string getPassword() const;
	std::string getEmail() const;

	// Setters
	void setUsername(const std::string& username);
	void setPassword(const std::string& password);
	void setEmail(const std::string& email);

	friend std::ostream& operator<<(std::ostream& stream, const User& user);
	friend bool operator==(const User& lUser, const User& rUser);

private:
	std::string m_username = "";
	std::string m_password = "";
	std::string m_email = "";
};