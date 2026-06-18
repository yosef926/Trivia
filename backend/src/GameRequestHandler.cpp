#include "GameRequestHandler.h"
#include "JsonResponsePacketSerializer.h"
#include "JsonResponsePacketDeserializer.h"

GameRequestHandler::GameRequestHandler(RequestHandlerFactory& HandlerFactory, const LoggedUser user, Game& game, GameManager& gameManager)
	: m_user(user),
	m_handlerFactory(HandlerFactory),
	m_game(game),
	m_gameManager(gameManager)
{
	this->m_handlers = {
	{ static_cast<uint8_t>(RequestsCode::GET_QUESTION), &GameRequestHandler::getQuestion },
	{ static_cast<uint8_t>(RequestsCode::SUBMIT_ANSWER), &GameRequestHandler::submitAnswer },
	{ static_cast<uint8_t>(RequestsCode::GET_GAME_RESULTS), &GameRequestHandler::getGameResults },
	{ static_cast<uint8_t>(RequestsCode::LEAVE_GAME), &GameRequestHandler::leaveGame },
	{ static_cast<uint8_t>(RequestsCode::LOGOUT), &GameRequestHandler::logout}
	};
}

GameRequestHandler::~GameRequestHandler()
{
	m_keepCounting.store(false);
	if (m_timerThread.joinable()) {
		m_timerThread.join();  // Wait for thread to exit before destroying object
	}
}

bool GameRequestHandler::isRequestRelevant(const RequestInfo& reqInfo)
{
	return reqInfo.id == static_cast<uint8_t>(RequestsCode::GET_QUESTION)
		|| reqInfo.id == static_cast<uint8_t>(RequestsCode::SUBMIT_ANSWER)
		|| reqInfo.id == static_cast<uint8_t>(RequestsCode::GET_GAME_RESULTS)
		|| reqInfo.id == static_cast<uint8_t>(RequestsCode::LEAVE_GAME)
		|| reqInfo.id == static_cast<uint8_t>(RequestsCode::LOGOUT);
}

RequestResult GameRequestHandler::handleRequest(const RequestInfo& reqInfo)
{
	if (isRequestRelevant(reqInfo))
	{
		auto it = m_handlers.find(reqInfo.id);
		if (it != m_handlers.end())
		{
			return (this->*(it->second))(reqInfo);
		}
	}
	std::cout << "Code message is not relevant for this GameRequestHandler" << std::endl;
	// ### Putting here 'LoginStatus::INVALID_CREDENTIALS', need to be fixed to be correct for the specific each request that can be not just login... (hint-->reqInfo.id) ###
	responses::LoginResponse response = { LoginStatus::INVALID_CREDENTIALS };
	std::vector<uint8_t> serialResponse = JsonResponsePacketSerializer::serializeResponse(response);
	RequestResult reqResult = { serialResponse, this };
	return reqResult;
}

RequestResult GameRequestHandler::getQuestion(const RequestInfo& reqInfo)
{
	GameData gameData = m_game.getPlayers().at(m_user);
	if (gameData.correctAnswerCount == 0 && gameData.wrongAnswerCount == 0)
	{
		// Start the clock
		startTimerThread();
	}

	Question question = m_gameManager.getQuestion(m_game.getGameId(), m_user);
	std::string questionText = question.getQuestion();
	std::vector<std::string> possibleAnswers = question.getPossibleAnswers();

	std::cout << "[QUESTION]: '" << questionText << "'" << std::endl;

	GetQuestionStatus responseStatus = GetQuestionStatus::FAIL;
	if (questionText != "")
	{
		responseStatus = GetQuestionStatus::SUCCESS;
	}

	std::map<uint8_t, std::string> answers;
	for (int i = 0; i < possibleAnswers.size(); i++)
	{
		// i + 1 because number of answer is not readed as index, more of a normal number, and normal "talk" of numbers do not start from zero
		answers[i] = possibleAnswers.at(i);
	}

	responses::GetQuestionResponse response = { responseStatus, questionText, answers };
	std::vector<uint8_t> serialResponse = JsonResponsePacketSerializer::serializeResponse(response);

	// Build handler
	IRequestHandler* reqHandler = this;
	RequestResult reqResult = { serialResponse, reqHandler };

	return reqResult;
}

RequestResult GameRequestHandler::submitAnswer(const RequestInfo& reqInfo)
{
	requests::SubmitAnswerRequest submitAnswerReq = JsonResponsePacketDeserializer::deserializeSubmitAnswerRequest(reqInfo.buffer);

	uint8_t correctAnswerId = m_gameManager.submitAnswer(m_game.getGameId(), m_user, submitAnswerReq.answerId);

	SubmitAnswerStatus responseStatus = (correctAnswerId == submitAnswerReq.answerId) ? SubmitAnswerStatus::SUCCESS : SubmitAnswerStatus::FAIL;

	responses::SubmitAnswerResponse response = { responseStatus, correctAnswerId };
	std::vector<uint8_t> serialResponse = JsonResponsePacketSerializer::serializeResponse(response);

	// Build handler
	IRequestHandler* reqHandler = this;
	RequestResult reqResult = { serialResponse, reqHandler };

	return reqResult;
}

RequestResult GameRequestHandler::getGameResults(const RequestInfo& reqInfo)
{
	if (m_keepCounting.exchange(false) == true)
	{
		float timeResult = m_finalElapsedTime.load();
		float avgAnswerTime = std::round((timeResult / m_game.getAmountOfQuestions()) * 100.0f) / 100.0f;
		m_game.updatePlayerTime(m_user, avgAnswerTime);
	}

	bool isGameOver = m_gameManager.isGameOver(m_game.getGameId());
	std::vector<PlayerResults> playerResults = {};

	if (isGameOver)
	{
		m_handlerFactory.getRoomManager().deleteRoom(m_game.getRoomId());
		playerResults = m_gameManager.getGameResults(m_game.getGameId());
		m_gameManager.submitGameStatsToDB(playerResults, m_handlerFactory.getStatisticsManager());
	}

	GetGameResultsStatus responseStatus = isGameOver ? GetGameResultsStatus::GAME_OVER : GetGameResultsStatus::GAME_STILL_RUNNING;

	responses::GetGameResultsResponse response = { responseStatus, playerResults };
	std::vector<uint8_t> serialResponse = JsonResponsePacketSerializer::serializeResponse(response);

	// Build handler
	IRequestHandler* reqHandler = this;
	if (isGameOver)
	{
		reqHandler = m_handlerFactory.createMenuRequestHandler(m_user);
	}
	RequestResult reqResult = { serialResponse, reqHandler };

	return reqResult;
}

RequestResult GameRequestHandler::leaveGame(const RequestInfo& reqInfo)
{
	LeaveGameStatus responseStatus = m_gameManager.removePlayer(m_game.getGameId(), m_user);
	responses::LeaveGameResponse response = { responseStatus };
	std::vector<uint8_t> serialResponse = JsonResponsePacketSerializer::serializeResponse(response);

	// Build handler
	IRequestHandler* reqHandler = this;
	if (responseStatus == LeaveGameStatus::SUCCESS)
	{
		reqHandler = m_handlerFactory.createMenuRequestHandler(m_user);
	}
	RequestResult reqResult = { serialResponse, reqHandler };

	return reqResult;
}

RequestResult GameRequestHandler::logout(const RequestInfo& reqInfo)
{
	LogoutStatus responseStatus = m_handlerFactory.getLoginManager().logout(m_user.getUsername());
	responses::LogoutResponse response = { responseStatus };
	std::vector<uint8_t> serialResponse = JsonResponsePacketSerializer::serializeResponse(response);

	// Build handler
	IRequestHandler* reqHandler;
	if (responseStatus == LogoutStatus::SUCCESS)
	{
		reqHandler = m_handlerFactory.createLoginRequestHandler();
	}
	else
	{
		reqHandler = this;
	}

	RequestResult reqResult = { serialResponse, reqHandler };

	return reqResult;
}


void GameRequestHandler::runTimer()
{
	int tenthsOfSecond = 0;
	m_keepCounting.store(true);

	while (m_keepCounting.load()) {
		std::this_thread::sleep_for(std::chrono::milliseconds(100));

		if (!m_keepCounting.load()) {
			break;
		}
		tenthsOfSecond++;
		m_finalElapsedTime.store(tenthsOfSecond / 10.0f);
	}
}

void GameRequestHandler::startTimerThread()
{
	m_timerThread = std::thread(&GameRequestHandler::runTimer, this);

}