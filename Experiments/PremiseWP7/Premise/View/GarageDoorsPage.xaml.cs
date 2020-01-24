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
using System.Windows.Data;
using Microsoft.Phone.Controls;
using GalaSoft.MvvmLight;
using System.Diagnostics;
using Premise;
using Premise.ViewModel;
using Premise.Model;


namespace Premise
{
    public partial class GarageDoorsPage : PhoneApplicationPage
    {
        // Constructor
        private System.Windows.Threading.DispatcherTimer timerRequery;
        public GarageDoorsPage()
        {
            Debug.WriteLine("MainViewModel");

            InitializeComponent();

            timerRequery = new System.Windows.Threading.DispatcherTimer();
            timerRequery.Interval = new TimeSpan(0, 0, 0, 0, 5000);
            timerRequery.Tick += (t, args) =>
            {
                ViewModelLocator.GarageDoorsViewModelStatic.Update();
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

        private void RefreshButtonClick(object sender, EventArgs e)
        {
            ViewModelLocator.GarageDoorsViewModelStatic.Update();
        }

        private void PhoneApplicationPage_Loaded(object sender, RoutedEventArgs e)
        {
            ViewModelLocator.GarageDoorsViewModelStatic.Update();
        }

    }

  
}