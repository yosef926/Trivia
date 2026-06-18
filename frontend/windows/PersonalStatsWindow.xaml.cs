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
    /// Interaction logic for PersonalStatsWindow.xaml
    /// </summary>
    public partial class PersonalStatsWindow : Window
    {
        private Communicator _communicator;
        protected bool _isNavigating = false;

        public PersonalStatsWindow(Communicator communicator)
        {
            InitializeComponent();
            _communicator = communicator;
            LoadStats();
        }

        private void LoadStats()
        {
            byte[] msg = Serializer.SerializeGetPersonalStatsRequest();
            _communicator.Send(msg);

            var (code, json) = _communicator.Receive();
            if (code == Constants.GET_PERSONAL_STATS_SUCCESS)
            {
                var stats = Newtonsoft.Json.Linq.JObject.Parse(json)["statistics"] as Newtonsoft.Json.Linq.JArray;
                if (stats != null && stats.Count >= 4)
                {
                    AvgTimeText.Text = "Avg Answer Time: " + stats[0];
                    CorrectAnsText.Text = "Correct Answers: " + stats[1];
                    TotalAnsText.Text = "Total Answers: " + stats[2];
                    GamesPlayedText.Text = "Games Played: " + stats[3];
                }
            }
            else
            {
                MessageBox.Show("No statistics found.");
            }
        }

        private void BackBtn_Click(object sender, RoutedEventArgs e)
        {
            _isNavigating = true;
            StatisticsWindow stats = new StatisticsWindow(_communicator);
            stats.Show();
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
