using Newtonsoft.Json.Linq;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Shapes;

namespace TriviaClient
{
    /// <summary>
    /// Interaction logic for RoomWindow.xaml
    /// </summary>
    public partial class RoomWindow : Window
    {
        private Communicator _communicator;
        private byte _roomId;
        private bool _isAdmin;
        private byte _totalQuestion;
        private byte _answerTimeout = 30; // default
        private Thread _refreshThread;
        private bool _isRunning = true;

        protected bool _isNavigating = false;

        public RoomWindow(Communicator communicator, byte roomId, bool isAdmin, byte totalQuestion)
        {
            InitializeComponent();
            _communicator = communicator;
            _roomId = roomId;
            _isAdmin = isAdmin;
            _totalQuestion = totalQuestion;
            StartBtn.IsEnabled = _isAdmin;
            CloseBtn.IsEnabled = _isAdmin;
            BackBtn.IsEnabled = !_isAdmin;
            LoadPlayers();
            StartRefreshThread();
        }

        private void LoadPlayers()
        {
            try
            {
                byte[] msg = Serializer.SerializeGetRoomStateRequest();
                _communicator.Send(msg);
                var (code, json) = _communicator.Receive();

                if (code == Constants.GET_ROOM_STATE_SUCCESS)
                {
                    var obj = Newtonsoft.Json.Linq.JObject.Parse(json);
                    byte roomStatus = (byte)obj["roomStatus"];

                    if (obj["answerTimeout"] != null)
                        _answerTimeout = (byte)obj["answerTimeout"];

                    if (roomStatus == Constants.ROOM_STATE_RUNNING)
                    {
                        _isRunning = false;
                        GoToGame();
                        return;
                    }
                    else if (roomStatus == Constants.ROOM_STATE_CLOSE_ROOM)
                    {
                        _isRunning = false;
                        GoToMenu();
                        return;
                    }

                    PlayersList.Items.Clear();
                    var players = obj["players"] as Newtonsoft.Json.Linq.JArray;
                    if (players != null)
                        foreach (var player in players)
                            PlayersList.Items.Add(player.ToString());
                }
                else
                {
                    _isRunning = false;
                    Dispatcher.Invoke(() =>
                    {
                        MessageBox.Show("Room was closed.");
                        _isNavigating = true;
                        MenuWindow menu = new MenuWindow(_communicator);
                        menu.Show();
                        this.Close();
                    });
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show("Error: " + ex.Message);
            }
        }

        private void StartRefreshThread()
        {
            _refreshThread = new Thread(() =>
            {
                while (_isRunning)
                {
                    Thread.Sleep(3000);
                    if (_isRunning)
                        Dispatcher.Invoke(() => LoadPlayers());
                }
            });
            _refreshThread.IsBackground = true;
            _refreshThread.Start();
        }

        protected override void OnClosed(EventArgs e)
        {
            _isRunning = false;
            base.OnClosed(e);
        }

        private void StartBtn_Click(object sender, RoutedEventArgs e)
        {
            byte[] msg = Serializer.SerializeStartGameRequest();
            _communicator.Send(msg);
            var (code, json) = _communicator.Receive();

            if (code == Constants.START_GAME_SUCCESS)
            {
                _isRunning = false;
                GoToGame();
            }
        }
        private void GoToGame()
        {
            Dispatcher.Invoke(() =>
            {
                _isNavigating = true;
                GameWindow game = new GameWindow(_communicator, _answerTimeout, _totalQuestion);
                game.Show();
                this.Close();
            });
        }

        private void GoToMenu()
        {
            _isRunning = false;
            Dispatcher.Invoke(() =>
            {
                MessageBox.Show("Room was closed.");
                _isNavigating = true;
                MenuWindow menu = new MenuWindow(_communicator);
                menu.Show();
                this.Close();
            });
        }

        private void CloseBtn_Click(object sender, RoutedEventArgs e)
        {
            byte[] msg = Serializer.SerializeCloseRoomRequest();
            _communicator.Send(msg);
            _communicator.Receive();
            _isRunning = false;
            _isNavigating = true;
            MenuWindow menu = new MenuWindow(_communicator);
            menu.Show();
            this.Close();
        }

        private void BackBtn_Click(object sender, RoutedEventArgs e)
        {
            byte[] msg = Serializer.SerializeLeaveRoomRequest();
            _communicator.Send(msg);
            _communicator.Receive();
            _isRunning = false;
            _isNavigating = true;
            MenuWindow menu = new MenuWindow(_communicator);
            menu.Show();
            this.Close();
        }

        protected override void OnClosing(System.ComponentModel.CancelEventArgs e)
        {
            base.OnClosing(e);
            if (_isNavigating) return; // don't logout on navigation

            try
            {
                if (_communicator != null)
                {
                    if (_isAdmin)
                    {
                        byte[] msg = Serializer.SerializeCloseRoomRequest();
                        _communicator.Send(msg);
                    }
                    else
                    {
                        byte[] msg = Serializer.SerializeLeaveRoomRequest();
                        _communicator.Send(msg);
                    }
                    _communicator.Send(Serializer.SerializeLogoutRequest());
                    _communicator.Receive();
                    _communicator.Close();
                }
            }
            catch { }
        }
    }
}
