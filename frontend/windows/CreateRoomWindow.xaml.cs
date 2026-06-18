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
    /// Interaction logic for CreateRoomWindow.xaml
    /// </summary>
    public partial class CreateRoomWindow : Window
    {
        private Communicator _communicator;
        protected bool _isNavigating = false;
        public CreateRoomWindow(Communicator communicator)
        {
            InitializeComponent();
            _communicator = communicator;
        }

        private void CreateBtn_Click(object sender, RoutedEventArgs e)
        {
            string roomName = RoomNameBox.Text.Trim();
            string maxPlayersStr = MaxPlayersBox.Text.Trim();
            string numQuestionsStr = NumQuestionsBox.Text.Trim();
            string timePerQStr = TimePerQuestionBox.Text.Trim();

            // Validation
            if (string.IsNullOrEmpty(roomName))
            {
                MessageBox.Show("Room name cannot be empty.");
                return;
            }
            if (!byte.TryParse(maxPlayersStr, out byte maxPlayers) || maxPlayers < 2 || maxPlayers > 10)
            {
                MessageBox.Show("Max players must be between 2 and 10.");
                return;
            }
            if (!byte.TryParse(numQuestionsStr, out byte numQuestions) || numQuestions < 1 || numQuestions > 10)
            {
                MessageBox.Show("Number of questions must be between 1 and 10.");
                return;
            }
            if (!ushort.TryParse(timePerQStr, out ushort timePerQ) || timePerQ < 1 || timePerQ > 30)
            {
                MessageBox.Show("Time per question must be between 1 and 30 seconds.");
                return;
            }

            // Generate temp room ID on client side by fetching rooms before and after
            byte[] msg = Serializer.SerializeCreateRoomRequest(roomName, maxPlayers, numQuestions, (byte)timePerQ);
            _communicator.Send(msg);
            var (code, json) = _communicator.Receive();

            if (code == Constants.CREATE_ROOM_SUCCESS)
            {
                // We don't know the ID yet — open RoomWindow and let GET_ROOM_STATE handle it
                // Pass 0 as placeholder, server doesn't need ID from admin side
                _isNavigating = true;
                RoomWindow roomWindow = new RoomWindow(_communicator, 0, true, numQuestions);
                roomWindow.Show();
                this.Close();
            }
            else
            {
                MessageBox.Show("Failed to create room. Code: " + code);
            }
        }

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
