#include <iostream>

#include "Question.h"

Question::Question(const std::string& question, const std::vector<std::string>& possibleAnswers, int correctAnswerId)
{
	m_question = question;
	m_possibleAnswers = possibleAnswers;
	m_correctAnswerId = correctAnswerId;
}

Question::~Question()
{
}


std::string Question::getQuestion()
{
	return m_question;
}


std::vector<std::string> Question::getPossibleAnswers()
{
	return m_possibleAnswers;
}


int Question::getCorrectAnswerId()
{
	return m_correctAnswerId;
}

bool operator==(const Question& lQuestion, const Question& rQuestion)
{
	return lQuestion.m_question == rQuestion.m_question;
}