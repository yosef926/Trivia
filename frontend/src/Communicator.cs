using System;
using System.Collections.Generic;
using System.Linq;
using System.Net.Sockets;
using System.Text;
using System.Threading.Tasks;
using System.Windows;

namespace TriviaClient
{
    public class Communicator
    {
        private TcpClient _client;
        private NetworkStream _stream;

        private const string SERVER_IP = "127.0.0.1";
        private const int SERVER_PORT = 22861;

        public void Connect()
        {
            try
            {
                _client = new TcpClient();
                _client.Connect(SERVER_IP, SERVER_PORT);
                _stream = _client.GetStream();
            }
            catch (Exception e)
            {
                throw new Exception("Connection failed: " + e.Message);
            }
        }

        public void Send(byte[] data)
        {
            try
            {
                _stream.Write(data, 0, data.Length);
            }
            catch (Exception e)
            {
                throw new Exception("Send failed: " + e.Message);
            }
        }

        public (int code, string json) Receive()
        {
            try
            {
                int code = _stream.ReadByte();

                byte[] sizeBytes = new byte[4];
                _stream.Read(sizeBytes, 0, 4);
                int size = (sizeBytes[0] << 24) | (sizeBytes[1] << 16) | (sizeBytes[2] << 8) | sizeBytes[3];

                byte[] payload = new byte[size];
                _stream.Read(payload, 0, size);
                string json = System.Text.Encoding.UTF8.GetString(payload);
                return (code, json);
            }
            catch (Exception e)
            {
                throw new Exception("Receive failed: " + e.Message);
            }
        }

        public void Close()
        {
            _stream?.Close();
            _client?.Close();
        }
    }
}
