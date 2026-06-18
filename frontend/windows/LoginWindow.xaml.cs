using System.Net.Sockets;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

using Newtonsoft.Json;


namespace TriviaClient
{
    /// <summary>
    /// Interaction logic for LoginWindow.xaml
    /// </summary>
    public partial class LoginWindow : Window
    {
        private Communicator _communicator;

        public LoginWindow()
        {
            InitializeComponent();
            _communicator = new Communicator();
            _communicator.Connect();
        }

        //incase of coming back from signup window or menu window (exit btn), we need to reconnect to the server
        public LoginWindow(Communicator communicator)
        {
            InitializeComponent();
            _communicator = communicator;
        }

        private void LoginBtn_Click(object sender, RoutedEventArgs e)
        {
            string username = UsernameBox.Text.Trim();
            string password = PasswordBox.Password;

            if (string.IsNullOrEmpty(username))
            {
                MessageBox.Show("Username cannot be empty.");
                return;
            }
            if (string.IsNullOrEmpty(password))
            {
                MessageBox.Show("Password cannot be empty.");
                return;
            }

            byte[] msg = Serializer.SerializeLoginRequest(username, password);
            _communicator.Send(msg);

            var (code, json) = _communicator.Receive();

            if (code == Constants.LOGIN_SUCCESS)
            {
                MenuWindow menu = new MenuWindow(_communicator);
                menu.Show();
                this.Close();
            }
            else
            {
                MessageBox.Show("Login failed. Code: " + code);
            }
        }

        private void SignupBtn_Click(object sender, RoutedEventArgs e)
        {
            SignupWindow signup = new SignupWindow(_communicator);
            signup.Show();
            this.Close();
        }
    }
}