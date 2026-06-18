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
    public class PlayerResultRow
    {
        public string Username { get; set; } = "";
        public byte CorrectCount { get; set; }
        public string AvgTime { get; set; }
    }


    /// <summary>
    /// Interaction logic for ResultWindow.xaml
    /// </summary>
    public partial class ResultWindow : Window
    {
        private Communicator _communicator;
        private Thread _pollThread;
        private volatile bool _isRunning = true;

        protected bool _isNavigating = false;

        public ResultWindow(Communicator communicator)
        {
            InitializeComponent();
            _communicator = communicator;
            StartPolling();
        }

        private void StartPolling()
        {
            _isRunning = true;
            _pollThread = new Thread(() =>
            {
                while (_isRunning)
                {
                    try
                    {
                        byte[] msg = Serializer.SerializeGetGameResultsRequest();
                        _communicator.Send(msg);

                        var (code, json) = _communicator.Receive();

                        if (code == Constants.GET_GAME_RESULTS_OVER)
                        {
                            _isRunning = false;
                            Dispatcher.BeginInvoke(new Action(() => ShowResults(json)));

                            break;
                        }
                        else
                        {
                            // Game is still running, rest before checking again
                            Thread.Sleep(2000);
                        }
                    }
                    catch (Exception ex)
                    {
                        System.Diagnostics.Debug.WriteLine($"Polling error: {ex.Message}");
                        _isRunning = false;
                        break;
                    }
                }
            });
            _pollThread.IsBackground = true;
            _pollThread.Start();
        }

        private void ShowResults(string json)
        {
            var obj = Newtonsoft.Json.Linq.JObject.Parse(json);
            var results = obj["results"] as Newtonsoft.Json.Linq.JArray;

            var rows = new List<PlayerResultRow>();
            if (results != null)
            {
                foreach (var r in results)
                {
                    rows.Add(new PlayerResultRow
                    {
                        Username = r["username"]?.ToString() ?? "",
                        CorrectCount = (byte)(r["correctAnswerCount"] ?? 0),
                        AvgTime = $"{(r["averageAnswerTime"] ?? 0):F2}s"
                    });
                }
            }

            ResultsList.ItemsSource = rows;

            if (rows.Count > 0)
            {
                var winner = rows[0];

                foreach (var row in rows)
                {
                    double currentAvgTime = double.Parse(row.AvgTime.Replace("s", ""));
                    double winnerAvgTime = double.Parse(winner.AvgTime.Replace("s", ""));

                    int currentScore = CalcScore(row.CorrectCount, currentAvgTime);
                    int winnerScore = CalcScore(winner.CorrectCount, winnerAvgTime);

                    if (currentScore > winnerScore)
                    {
                        winner = row;
                    }
                }

                WinnerText.Text = "WINNER: " + winner.Username.ToUpper();
            }
        }

        private void BackBtn_Click(object sender, RoutedEventArgs e)
        {
            byte[] msg = Serializer.SerializeLeaveGameRequest();
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
                    _communicator.Send(Serializer.SerializeLogoutRequest());
                    _communicator.Receive();
                    _communicator.Close();
                }
            }
            catch { }
        }

        private int CalcScore(int correctCount, double avgTime)
        {
            if (avgTime < 0.1)
            {
                avgTime = 0.1;
            }

            int score = (correctCount * 100) + (int)(correctCount * (10.0 / avgTime));

            return score;
        }
    }
}
