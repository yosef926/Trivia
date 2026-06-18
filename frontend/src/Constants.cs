using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace TriviaClient
{
    public class Constants
    {
        public const string SERVER_IP = "127.0.0.1";
        public const int SERVER_PORT = 22861;

        // Request codes
        public const byte LOGIN = 100;
        public const byte SIGNUP = 110;
        public const byte LOGOUT = 120;
        public const byte GET_HIGH_SCORE = 130;
        public const byte GET_ROOM = 140;
        public const byte GET_PLAYERS_IN_ROOM = 150;
        public const byte JOIN_ROOM = 160;
        public const byte CREATE_ROOM = 170;
        public const byte GET_PERSONAL_STATS = 180;
        public const byte CLOSE_ROOM = 190;
        public const byte START_GAME = 200;
        public const byte GET_ROOM_STATE = 210;
        public const byte LEAVE_ROOM = 220;
        public const byte LEAVE_GAME = 230;
        public const byte GET_QUESTION = 240;
        public const byte SUBMIT_ANSWER = 245;
        public const byte GET_GAME_RESULTS = 250;

        // Response codes - success
        public const byte LOGIN_SUCCESS = 101;
        public const byte LOGIN_ALREADY_LOGGED = 102;
        public const byte LOGIN_INVALID_CREDENTIALS = 103;

        public const byte SIGNUP_SUCCESS = 111;
        public const byte SIGNUP_USERNAME_EXISTS = 112;

        public const byte LOGOUT_SUCCESS = 121;
        public const byte LOGOUT_FAIL = 122;

        public const byte GET_HIGH_SCORE_SUCCESS = 131;
        public const byte GET_HIGH_SCORE_FAIL = 132;

        public const byte GET_ROOM_SUCCESS = 141;
        public const byte GET_ROOM_FAIL = 142;

        public const byte GET_PLAYERS_SUCCESS = 151;
        public const byte GET_PLAYERS_FAIL = 152;

        public const byte JOIN_ROOM_SUCCESS = 161;
        public const byte JOIN_ROOM_FAIL = 162;

        public const byte CREATE_ROOM_SUCCESS = 171;
        public const byte CREATE_ROOM_FAIL = 172;

        public const byte GET_PERSONAL_STATS_SUCCESS = 181;
        public const byte GET_PERSONAL_STATS_FAIL = 182;

        public const byte CLOSE_ROOM_SUCCESS = 191;
        public const byte CLOSE_ROOM_FAIL = 192;

        public const byte START_GAME_SUCCESS = 201;
        public const byte START_GAME_FAIL = 202;

        public const byte GET_ROOM_STATE_SUCCESS = 211;
        public const byte GET_ROOM_STATE_FAIL = 212;

        public const byte LEAVE_ROOM_SUCCESS = 221;
        public const byte LEAVE_ROOM_FAIL = 222;

        public const byte LEAVE_GAME_SUCCESS = 231;

        public const byte GET_QUESTION_SUCCESS = 241;
        public const byte GET_QUESTION_FAIL = 242;

        public const byte SUBMIT_ANSWER_SUCCESS = 246;
        public const byte SUBMIT_ANSWER_FAIL = 247;
        
        public const byte GET_GAME_RESULTS_RUNNING = 251;
        public const byte GET_GAME_RESULTS_OVER = 252;

        // Error codes
        public const byte ERROR_LOGIN = 109;
        public const byte ERROR_SIGNUP = 119;
        public const byte ERROR_LOGOUT = 129;
        public const byte ERROR_GET_HIGH_SCORE = 139;
        public const byte ERROR_GET_ROOM = 149;
        public const byte ERROR_GET_PLAYERS = 159;
        public const byte ERROR_JOIN_ROOM = 169;
        public const byte ERROR_CREATE_ROOM = 179;
        public const byte ERROR_GET_PERSONAL_STATS = 189;

        //Room status
        public const byte ROOM_STATE_WAITING_FOR_PLAYERS = 30;
        public const byte ROOM_STATE_RUNNING = 31;
        public const byte ROOM_STATE_CLOSE_ROOM = 32;
    }
}
