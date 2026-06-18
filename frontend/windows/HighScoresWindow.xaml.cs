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
    /// Interaction logic for HighScoresWindow.xaml
    /// </summary>
    public partial class HighScoresWindow : Window
    {
        private Communicator _communicator;
        private string[] medals = { "1st", "2nd", "3rd" };
        private string[] colors = { "#f1c40f", "#95a5a6", "#cd6133" };
        protected bool _isNavigating = false;
        public HighScoresWindow(Communicator communicator)
        {
            InitializeComponent();
            _communicator = communicator;
            LoadHighScores();
        }

        private void LoadHighScores()
        {
            byte[] msg = Serializer.SerializeGetHighScoreRequest();
            _communicator.Send(msg);

            var (code, json) = _communicator.Receive();
            if (code == Constants.GET_HIGH_SCORE_SUCCESS)
            {
                var scores = Newtonsoft.Json.Linq.JObject.Parse(json)["statistics"] as Newtonsoft.Json.Linq.JArray;
                if (scores != null)
                {
                    for (int i = 0; i < Math.Min(scores.Count, 3); i++)
                    {
                        Border border = new Border
                        {
                            Background = new System.Windows.Media.SolidColorBrush(
                                (System.Windows.Media.Color)System.Windows.Media.ColorConverter.ConvertFromString(colors[i])),
                            CornerRadius = new CornerRadius(8),
                            Padding = new Thickness(10),
                            Margin = new Thickness(0, 5, 0, 5)
                        };
                        TextBlock text = new TextBlock
                        {
                            Text = $"{medals[i]} {scores[i]}",
                            Foreground = System.Windows.Media.Brushes.White,
                            FontSize = 15
                        };
                        border.Child = text;
                        ScoresPanel.Children.Add(border);
                    }
                }
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
