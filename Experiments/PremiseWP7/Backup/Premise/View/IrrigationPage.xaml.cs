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
using Premise.ViewModel;

namespace Premise.View
{
    public partial class IrrigationPage : PhoneApplicationPage
    {
        private System.Windows.Threading.DispatcherTimer timerRequery;
        public IrrigationPage()
        {
            InitializeComponent();

            timerRequery = new System.Windows.Threading.DispatcherTimer();
            timerRequery.Interval = new TimeSpan(0, 0, 0, 0, 5000);
            timerRequery.Tick += (t, args) =>
            {
                ViewModelLocator.IrrigationViewModelStatic.Update();
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

        private void PhoneApplicationPage_Loaded(object sender, RoutedEventArgs e)
        {
            ViewModelLocator.IrrigationViewModelStatic.Update();
        }

    }
}