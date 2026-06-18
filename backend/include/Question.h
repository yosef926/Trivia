#pragma once

#include <string>
#include <vector>

class Question
{
public:
	Question(const std::string& question, const std::vector<std::string>& possibleAnswers, int correctAnswerIndex);
	~Question();

	std::string getQuestion();
	std::vector<std::string> getPossibleAnswers();
	int getCorrectAnswerId();
	friend bool operator==(const Question& lQuestion, const Question& rQuestion);

private:
	std::string m_question;
	std::vector<std::string> m_possibleAnswers;
	int m_correctAnswerId;
};