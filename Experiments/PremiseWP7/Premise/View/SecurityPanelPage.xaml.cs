using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Documents;
using System.Windows.Input;
using System.Diagnostics;
using System.Windows.Media.Animation;
using System.Windows.Shapes;
using Microsoft.Phone.Controls;
using System.Windows.Threading;
using GalaSoft.MvvmLight.Messaging;
using Premise;
using Premise.Model;
using Premise.ViewModel;

namespace Premise
{
    public partial class SecurityPanelPage : PhoneApplicationPage
    {
        // Constructor
        private System.Windows.Threading.DispatcherTimer timerRequery;
        public SecurityPanelPage()
        {
            InitializeComponent();

            timerRequery = new System.Windows.Threading.DispatcherTimer();
            timerRequery.Interval = new TimeSpan(0, 0, 0, 0, 5000);
            timerRequery.Tick += (t, args) =>
            {
                ViewModelLocator.SecurityPanelViewModelStatic.Update();
            };
            timerRequery.Start();
        }

        private void StopTimer()
        {
            timerRequery.Stop();
        }

        private void PhoneApplicationPage_Unloaded(object sender, RoutedEventArgs e)
        {
            StopTimer();
        }

        private bool ShowLog = false;

        private void LEDButton_Click(object sender, RoutedEventArgs e)
        {
            ShowLog = !ShowLog;
            if (ShowLog)
            {
                LED.Height = LEDRectangle.Height = 525;
                LEDButton.Content = "↑";
            }
            else
            {
                LED.Height = LEDRectangle.Height = 100;
                LEDButton.Content = "↓";
            }
            MoreInfoTextBlock.Height = LEDRectangle.Height - 6 - SecState.Height;
        }
        
    }
}