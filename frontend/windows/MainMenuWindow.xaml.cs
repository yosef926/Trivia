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
    /// Interaction logic for MenuWindow.xaml
    /// </summary>
    public partial class MenuWindow : Window
    {
        private Communicator _communicator;
        protected bool _isNavigating = false;
        public MenuWindow(Communicator communicator)
        {
            InitializeComponent();
            _communicator = communicator;
        }

        private void CreateRoomBtn_Click(object sender, RoutedEventArgs e)
        {
            _isNavigating = true;
            CreateRoomWindow createRoom = new CreateRoomWindow(_communicator);
            createRoom.Show();
            this.Close();
        }

        private void JoinRoomBtn_Click(object sender, RoutedEventArgs e)
        {
            _isNavigating = true;
            JoinRoomWindow joinRoom = new JoinRoomWindow(_communicator);
            joinRoom.Show();
            this.Close();
        }

        private void StatisticsBtn_Click(object sender, RoutedEventArgs e)
        {
            _isNavigating = true;
            StatisticsWindow stats = new StatisticsWindow(_communicator);
            stats.Show();
            this.Close();
        }

        private void ExitBtn_Click(object sender, RoutedEventArgs e)
        {
            _isNavigating = true;
            _communicator.Send(Serializer.SerializeLogoutRequest());
            _communicator.Receive();
            LoginWindow stats = new LoginWindow(_communicator);
            stats.Show();
            this.Close();
            //_communicator.Close();
            //Application.Current.Shutdown();
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
