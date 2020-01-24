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
using System.Diagnostics;
using GalaSoft.MvvmLight.Helpers;
using GalaSoft.MvvmLight.Messaging;
using GalaSoft.MvvmLight.Threading;
using GalaSoft.MvvmLight.Command;
//using Premise.Model;
using Premise.ViewModel;


namespace Premise
{
    public partial class MainPage : PhoneApplicationPage
    {
        // Constructor
        public MainPage()
        {
            InitializeComponent();

            // Set the data context of the listbox control to the sample data
            //DataContext = App.ViewModel;
            this.Loaded += new RoutedEventHandler(MainPage_Loaded);

            Messenger.Default.Register<MainPageNavigateMessage>(this, (action) => MainPageNavigateHandler(action.Page));


        }

        private System.Windows.Threading.DispatcherTimer timerRequery;
        private void StopTimer()
        {
            timerRequery.Stop();
        }

        private void StartTimer()
        {
            timerRequery = new System.Windows.Threading.DispatcherTimer();
            timerRequery.Interval = new TimeSpan(0, 0, 0, 5);
            timerRequery.Tick += (t, args) =>
            {
                Debug.WriteLine("MainPage Tick - calling Update()");
                ViewModelLocator.MainViewModelStatic.Update();
            };
            timerRequery.Start();
        }

        // Load data for the ViewModel Items
        private void MainPage_Loaded(object sender, RoutedEventArgs e)
        {
            Debug.WriteLine("MainPage_Loaded - calling Update()");
            ViewModelLocator.MainViewModelStatic.Update();
            StartTimer();
        }

        private void PhoneApplicationPage_Unloaded(object sender, RoutedEventArgs e)
        {
            StopTimer();
        }

        private void PhoneApplicationPage_Loaded(object sender, RoutedEventArgs e)
        {
        }

        private void ListBox_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {

        }

        private void button1_Click(object sender, RoutedEventArgs e)
        {
        }

        private void QuickItemsListBox_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
        }

        void MainPageNavigateHandler(String page)
        {
            if (page != null && page != "")
                NavigationService.Navigate(new Uri(page, UriKind.Relative));
   
            // Bug in SL listbox prevents SelectedIndex = -1 from unselected.
            // Workaround is to use DispatcherBeginInvoke to do it async. Found
            // work around here:
            // http://sharplogic.com/blogs/rdavis/PermaLink,guid,2f5bbfa1-4878-490f-967d-bf00bc04dfde.aspx
            Dispatcher.BeginInvoke(() => { QuickItemsListBox.SelectedIndex = -1; });
            Dispatcher.BeginInvoke(() => { SettingsItemsListBox.SelectedIndex = -1; });
        }

        private void PhoneApplicationPage_LayoutUpdated(object sender, EventArgs e)
        {

        }

        private void LightingListBox_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
        }

        private void GDOListBox_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
        }

    }

    class MainPageNavigateMessage : MessageBase
    {
        public String Page = "";
        public MainPageNavigateMessage(String page)
        {
            this.Page = page;
        }
    }

}
