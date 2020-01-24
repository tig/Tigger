using System;
using System.Net;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Documents;
using System.Windows.Ink;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Animation;
using System.Windows.Shapes;

namespace Premise.Model
{
    public class Settings
    {
        public Settings()
        {
        }
        public String PageName { get; set; }
        public String Description { get; set; }
    }

    public class ConnectionSettings 
    {
        public ConnectionSettings() 
        {
            Host = "home.kindel.net";
            Port = 86;
            SSL = false;
            Username = "premise";
            Password = "";
        }

        public String Host { get; set; }
        public int Port { get; set; }
        public bool SSL { get; set; }

        public String Username { get; set; }
        public String Password { get; set; }
    }
}
