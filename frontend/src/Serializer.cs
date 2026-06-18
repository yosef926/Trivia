using Newtonsoft.Json;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace TriviaClient
{
    public class Serializer
    {
        public static byte[] SerializeCreateRoomRequest(string roomName, byte maxUsers, byte questionsCount, byte answerTimeout)
        {
            return BuildRequest(Constants.CREATE_ROOM, new { roomName, maxUsers, questionsCount, answerTimeout });
        }

        public static byte[] SerializeGetRoomsRequest()
        {
            byte code = Constants.GET_ROOM;
            byte[] buffer = new byte[5];
            buffer[0] = code;
            buffer[1] = 0;
            buffer[2] = 0;
            buffer[3] = 0;
            buffer[4] = 0;
            return buffer;
        }

        public static byte[] SerializeGetPlayersInRoomRequest(byte roomId)
        {
            return BuildRequest(Constants.GET_PLAYERS_IN_ROOM, new { roomId });
        }

        public static byte[] SerializeJoinRoomRequest(byte roomId)
        {
            return BuildRequest(Constants.JOIN_ROOM, new { roomId });
        }

        public static byte[] SerializeGetPersonalStatsRequest()
        {
            return BuildRequest(Constants.GET_PERSONAL_STATS, new { });
        }

        public static byte[] SerializeGetHighScoreRequest()
        {
            return BuildRequest(Constants.GET_HIGH_SCORE, new { });
        }
        public static byte[] SerializeLoginRequest(string username, string password)
        {
            return BuildRequest(Constants.LOGIN, new { username, password });
        }

        public static byte[] SerializeSignupRequest(string username, string password, string email)
        {
            return BuildRequest(Constants.SIGNUP, new { username, password, email });
        }

        public static byte[] SerializeGetRoomStateRequest()
        {
            return BuildRequest(Constants.GET_ROOM_STATE, new { });
        }

        public static byte[] SerializeCloseRoomRequest()
        {
            return BuildRequest(Constants.CLOSE_ROOM, new { });
        }

        public static byte[] SerializeLeaveRoomRequest()
        {
            return BuildRequest(Constants.LEAVE_ROOM, new { });
        }

        public static byte[] SerializeLogoutRequest()
        {
            return BuildRequest(Constants.LOGOUT, new { });
        }

        public static byte[] SerializeGetQuestionRequest()
        {
            return BuildRequest(Constants.GET_QUESTION, new { });
        }

        public static byte[] SerializeSubmitAnswerRequest(byte answerId)
        {
            return BuildRequest(Constants.SUBMIT_ANSWER, new { answerId });
        }

        public static byte[] SerializeGetGameResultsRequest()
        {
            return BuildRequest(Constants.GET_GAME_RESULTS, new { });
        }

        public static byte[] SerializeLeaveGameRequest()
        {
            return BuildRequest(Constants.LEAVE_GAME, new { });
        }

        public static byte[] SerializeStartGameRequest()
        {
            return BuildRequest(Constants.START_GAME, new { });
        }

        private static byte[] BuildRequest(byte code, object data)
        {
            string json = JsonConvert.SerializeObject(data);
            byte[] payload = Encoding.UTF8.GetBytes(json);
            int size = payload.Length;

            byte[] buffer = new byte[1 + 4 + size];
            buffer[0] = code;
            buffer[1] = (byte)((size >> 24) & 0xFF);
            buffer[2] = (byte)((size >> 16) & 0xFF);
            buffer[3] = (byte)((size >> 8) & 0xFF);
            buffer[4] = (byte)(size & 0xFF);
            Array.Copy(payload, 0, buffer, 5, size);

            return buffer;
        }
    }
}
