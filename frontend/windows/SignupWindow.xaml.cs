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
    /// Interaction logic for SignupWindow.xaml
    /// </summary>
    public partial class SignupWindow : Window
    {
        private Communicator _communicator;

        public SignupWindow(Communicator communicator)
        {
            InitializeComponent();
            _communicator = communicator;
        }

        private void SignupBtn_Click(object sender, RoutedEventArgs e)
        {
            string username = UsernameBox.Text.Trim();
            string password = PasswordBox.Password;
            string email = EmailBox.Text.Trim();

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
            if (!email.EndsWith("@gmail.com"))
            {
                MessageBox.Show("Email must end with @gmail.com.");
                return;
            }

            byte[] msg = Serializer.SerializeSignupRequest(username, password, email);
            _communicator.Send(msg);

            var (code, json) = _communicator.Receive();

            if (code == Constants.SIGNUP_SUCCESS)
            {
                MessageBox.Show("Signup successful!");
                MenuWindow login = new MenuWindow(_communicator);
                login.Show();
                this.Close();
            }
            else
            {
                MessageBox.Show("Signup failed. Code: " + code);
            }
        }

        private void BackBtn_Click(object sender, RoutedEventArgs e)
        {
            LoginWindow login = new LoginWindow(_communicator);
            login.Show();
            this.Close();
        }
    }
}
