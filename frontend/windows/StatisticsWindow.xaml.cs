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
    /// Interaction logic for StatisticsWindow.xaml
    /// </summary>
    public partial class StatisticsWindow : Window
    {
        private Communicator _communicator;
        protected bool _isNavigating = false;

        public StatisticsWindow(Communicator communicator)
        {
            InitializeComponent();
            _communicator = communicator;
        }

        private void PersonalBtn_Click(object sender, RoutedEventArgs e)
        {
            _isNavigating = true;
            PersonalStatsWindow stats = new PersonalStatsWindow(_communicator);
            stats.Show();
            this.Close();
        }

        private void HighScoresBtn_Click(object sender, RoutedEventArgs e)
        {
            _isNavigating = true;
            HighScoresWindow highScores = new HighScoresWindow(_communicator);
            highScores.Show();
            this.Close();
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
