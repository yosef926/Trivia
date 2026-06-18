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
    /// Interaction logic for GameWindow.xaml
    /// </summary>
    public partial class GameWindow : Window
    {
        private Communicator _communicator;
        private Dictionary<int, byte> _answerIdMap = new Dictionary<int, byte>();
        private byte _correctCount = 0;
        private byte _answeredCount = 0;
        private byte _totalQuestion;
        private byte _answerTimeout;
        private System.Threading.Timer? _questionTimer;
        private int _timeLeft;

        protected bool _isNavigating = false;


        public GameWindow(Communicator communicator, byte answerTimeout, byte totalQuestion)
        {
            InitializeComponent();
            _communicator = communicator;
            _answerTimeout = answerTimeout;
            _totalQuestion = totalQuestion;
            StartCountdown();
        }

        private void StartCountdown()
        {
            int countdown = 3;
            TimerText.Text = countdown.ToString();
            QuestionText.Text = "GET READY...";

            SetAnswerButtonsEnabled(false);

            System.Windows.Threading.DispatcherTimer timer = new System.Windows.Threading.DispatcherTimer();
            timer.Interval = TimeSpan.FromSeconds(1);

            timer.Tick += (sender, e) =>
            {
                countdown--;
                TimerText.Text = countdown.ToString();

                if (countdown == 0)
                {
                    timer.Stop();

                    SetAnswerButtonsEnabled(true);
                    LoadQuestion();
                }
            };

            timer.Start();
        }

        private void SetAnswerButtonsEnabled(bool isEnabled)
        {
            Answer1Btn.IsEnabled = isEnabled;
            Answer2Btn.IsEnabled = isEnabled;
            Answer3Btn.IsEnabled = isEnabled;
            Answer4Btn.IsEnabled = isEnabled;
        }


        private void LoadQuestion()
        {
            Task.Run(() =>
            {
                try
                {
                    byte[] msg = Serializer.SerializeGetQuestionRequest();
                    _communicator.Send(msg);
                    return _communicator.Receive();
                }
                catch (Exception ex)
                {
                    return ((byte)0, ex.Message);
                }
            }).ContinueWith(task =>
            {
                Dispatcher.Invoke(() =>
                {
                    var (code, json) = task.Result;

                    if (code == Constants.GET_QUESTION_SUCCESS && _answeredCount < _totalQuestion)
                    {
                        var obj = Newtonsoft.Json.Linq.JObject.Parse(json);
                        QuestionText.Text = obj["question"]?.ToString() ?? "";

                        var answersArray = obj["answers"] as Newtonsoft.Json.Linq.JArray;
                        _answerIdMap.Clear();
                        var buttons = new[] { Answer1Btn, Answer2Btn, Answer3Btn, Answer4Btn };

                        int i = 0;
                        if (answersArray != null)
                        {
                            foreach (var item in answersArray)
                            {
                                if (i >= 4) break;

                                var innerArray = item as Newtonsoft.Json.Linq.JArray;
                                if (innerArray != null && innerArray.Count >= 2)
                                {
                                    byte answerId = innerArray[0].Value<byte>();
                                    string answerText = innerArray[1].ToString();

                                    buttons[i].Content = answerText;
                                    _answerIdMap[i] = answerId;
                                    i++;
                                }
                            }
                        }

                        RemainingText.Text = "QUESTION IN PROGRESS";
                        CorrectText.Text = $"CORRECT: {_correctCount}";
                        StartQuestionTimer();
                    }
                    else
                    {
                        GoToResults();
                    }
                });
            });
        }

        private void AnswerBtn_Click(object sender, RoutedEventArgs e)
        {
            _questionTimer?.Dispose();

            Button clicked = (Button)sender;
            int index = int.Parse(clicked.Tag.ToString());
            byte answerId = _answerIdMap[index];

            Task.Run(() =>
            {
                try
                {
                    byte[] msg = Serializer.SerializeSubmitAnswerRequest(answerId);
                    _communicator.Send(msg);
                    return _communicator.Receive();
                }
                catch (Exception ex)
                {
                    return ((byte)0, ex.Message);
                }
            }).ContinueWith(task =>
            {
                Dispatcher.Invoke(() =>
                {
                    var (code, json) = task.Result;

                    if (code == Constants.SUBMIT_ANSWER_SUCCESS)
                    {
                        _correctCount++;
                    }
                    _answeredCount++;

                    CorrectText.Text = $"CORRECT: {_correctCount}";

                    if (_answeredCount >= _totalQuestion)
                    {
                        GoToResults();
                    }
                    else
                    {
                        LoadQuestion();
                    }
                });
            });
        }

        private void GoToResults()
        {
            _isNavigating = true;
            ResultWindow results = new ResultWindow(_communicator);
            results.Show();
            this.Close();
        }

        private void StartQuestionTimer()
        {
            _timeLeft = _answerTimeout;
            TimerText.Text = _timeLeft.ToString();

            _questionTimer?.Dispose();
            _questionTimer = new System.Threading.Timer(_ =>
            {
                _timeLeft--;
                Dispatcher.Invoke(() => TimerText.Text = _timeLeft.ToString());

                if (_timeLeft <= 0)
                {
                    _questionTimer?.Dispose();
                    Dispatcher.Invoke(() =>
                    {
                        // There is no id with 4(all id's are 0-3), so it will be 100% wrong --> Defualt case
                        byte[] msg = Serializer.SerializeSubmitAnswerRequest(4);
                        _communicator.Send(msg);
                        _communicator.Receive();
                        _answeredCount++;
                        CorrectText.Text = $"CORRECT: {_correctCount}";
                        LoadQuestion();
                    });
                }
            }, null, 1000, 1000);
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
