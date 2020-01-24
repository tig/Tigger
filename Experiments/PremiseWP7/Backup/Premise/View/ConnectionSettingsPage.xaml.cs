using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Animation;
using System.Windows.Shapes;
using Microsoft.Phone.Controls;
using GalaSoft.MvvmLight;
using GalaSoft.MvvmLight.Messaging;
using GalaSoft.MvvmLight.Threading;
using Premise.Model;
using Premise.ViewModel;
using System.Diagnostics;

namespace Premise.View
{
    public partial class ConnectionSettingsPage : PhoneApplicationPage
    {
        public ConnectionSettingsPage()
        {
            InitializeComponent();
        }

        private void btnTestConnection_Click(object sender, EventArgs e)
        {
            Host.GetBindingExpression(TextBox.TextProperty).UpdateSource();
            Port.GetBindingExpression(TextBox.TextProperty).UpdateSource();
            checkBoxSSL.GetBindingExpression(CheckBox.IsCheckedProperty).UpdateSource();
            Username.GetBindingExpression(TextBox.TextProperty).UpdateSource();
            Password.GetBindingExpression(PasswordBox.PasswordProperty).UpdateSource();
            
            int port = 0;
            if (int.TryParse(Port.Text, out port) == false || port < 1)
            {
                MessageBox.Show(String.Format("A port of '{0}' is invalid. Port must be a positive integer.", port));
                return;
            }

            tbConnected.Text = "Testing...";

            PremiseServer.Instance.ConnectComplete += (s, ea) =>
                {
                    DispatcherHelper.CheckBeginInvokeOnUI(() =>
                    {
                        if (ea.Succeeded)
                        {
                            tbConnected.Text = "Connected to " + ea.Response.ToString();
                        }
                        else
                        {
                            tbConnected.Text = "Cound not connect: " + ea.Response.ToString();
                        }
                    });
                };

            PremiseServer.Instance.Connect();
        }

        private void PhoneApplicationPage_Unloaded(object sender, RoutedEventArgs e)
        {

        }

        private void PhoneApplicationPage_LostFocus(object sender, RoutedEventArgs e)
        {
        }

        private void PhoneApplicationPage_BackKeyPress(object sender, System.ComponentModel.CancelEventArgs e)
        {
            Debug.WriteLine("BackKeyPress");
            PremiseServer serv = PremiseServer.Instance;
            serv.SaveSettings(true);
            serv.Connect();
        }
    }
}