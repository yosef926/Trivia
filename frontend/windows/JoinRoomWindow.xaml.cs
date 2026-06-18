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
    /// Interaction logic for JoinRoomWindow.xaml
    /// </summary>
    public partial class JoinRoomWindow : Window
    {
        private Communicator _communicator;
        protected bool _isNavigating = false;
        private Newtonsoft.Json.Linq.JArray _roomsArray = null;

        // Store room IDs alongside display strings
        private List<byte> _roomIds = new List<byte>();

        private Thread _refreshThread;
        private bool _isRunning = true;

        public JoinRoomWindow(Communicator communicator)
        {
            InitializeComponent();
            _communicator = communicator;
            LoadRooms();
            StartRefreshThread();
        }

        private void LoadRooms()
        {
            _roomIds.Clear();

            byte[] msg = Serializer.SerializeGetRoomsRequest();
            _communicator.Send(msg);
            var (code, json) = _communicator.Receive();

            if (code == Constants.GET_ROOM_SUCCESS)
            {
                RoomsList.Items.Clear();
                var roomsArray = Newtonsoft.Json.Linq.JObject.Parse(json)["rooms"] as Newtonsoft.Json.Linq.JArray;
                
                if (roomsArray != null && roomsArray != _roomsArray)
                {
                    _roomsArray = roomsArray;
                }

                if (roomsArray != null)
                {
                    foreach (var room in roomsArray)
                    {
                        _roomIds.Add((byte)room["id"]);
                        string display = $" {room["name"]} | Players: {room["maxPlayers"]} | Questions: {room["numOfQuestionsInGame"]} | Time: {room["timePerQuestions"]}s";
                        if ((byte)room["status"] != Constants.ROOM_STATE_RUNNING)
                        {
                            RoomsList.Items.Add(display);
                        }
                    }
                }
            }
        }

        private void JoinBtn_Click(object sender, RoutedEventArgs e)
        {
            if (RoomsList.SelectedItem == null) return;
            byte roomId = _roomIds[RoomsList.SelectedIndex];

            byte[] msg = Serializer.SerializeJoinRoomRequest(roomId);
            _communicator.Send(msg);
            var (code, json) = _communicator.Receive();

            if (code == Constants.JOIN_ROOM_SUCCESS)
            {
                _isNavigating = true;
                byte numOfQuestionsInGame = 0;
                foreach (var room1 in _roomsArray)
                {
                    if ((byte)room1["id"] == roomId)
                    {
                        numOfQuestionsInGame = (byte)room1["numOfQuestionsInGame"];
                    }
                }
                RoomWindow room = new RoomWindow(_communicator, roomId, false, numOfQuestionsInGame);
                room.Show();
                this.Close();
            }
            else
            {
                MessageBox.Show("Room is full");
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
                        Dispatcher.Invoke(() => LoadRooms());
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

        private void RefreshBtn_Click(object sender, RoutedEventArgs e) => LoadRooms();

        private void BackBtn_Click(object sender, RoutedEventArgs e)
        {
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
                    _communicator.Send(Serializer.SerializeLogoutRequest());
                    _communicator.Receive();
                    _communicator.Close();
                }
            }
            catch { }
        }
    }
}
