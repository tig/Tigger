using System;
using System.Threading;
using System.ComponentModel;
using System.Collections.Generic;
using System.Diagnostics;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Shapes;
using System.Collections.ObjectModel;
using GalaSoft.MvvmLight;
using GalaSoft.MvvmLight.Messaging;
using GalaSoft.MvvmLight.Threading;
using Premise;
using Premise.Model;

namespace Premise.ViewModel
{
    public class MainViewModel : ViewModelBase
    {
        public MainViewModel()
        {
            Debug.WriteLine("MainViewModel() - Thread: {0:X}", Thread.CurrentThread.ManagedThreadId);

            if (IsInDesignMode)
            {
                // Code runs in Blend --> create design time data.
                Visible = Visibility.Visible;
                PageVisible = Visibility.Visible;
            }
            else
            {
                Visible = Visibility.Collapsed;
                PageVisible = Visibility.Collapsed;
                // Code runs "for real": Connect to service, etc...

                ApplicationTitle = "";
                PageName = "Quick Access";

                // Create our server instance. It will not talk on the network until we tell it to (Server.Connect).

                //Host = "home.kindel.net";
                //Port = 86;
                //SSL = false;
                //Username = "premise";
                //Password = "";

                // Invoke connect async method. When our delegate is called the connection attempt 
                // is complete.  It's either a success (Response is the DisplayName of Home) or a failure 
                // (Response is the error).
                //

                SettingsItems = new ObservableCollection<SettingsViewModelBase>();

                SettingsViewModelBase set = null;
                SettingsItems.Add(set = ViewModelLocator.ConnectionSettingsViewModelStatic);
                set.Page = "/View/ConnectionSettingsPage.xaml";
                SettingsItems.Add(set = new SettingsViewModelBase());
                SettingsItems.Add(set = new SettingsViewModelBase());

            }
        }

        /// <summary>
        /// List of objects used by this view
        /// </summary>
        private List<PremiseObject> objectList = new List<PremiseObject>();

        /// <summary>
        /// A collection for Settings Pages objects.
        /// </summary>
        public ObservableCollection<SettingsViewModelBase> SettingsItems { get; private set; }


        /// <summary>
        /// This should be called once at startup. This vm (MainViewModel) holds the single
        /// instance of PremiseServer and manages it's lifetime.
        /// </summary>
        public void InitServer()
        {
            Debug.WriteLine("MainViewModel.InitServer() - {0} Thread: {1:X}", PremiseServer.Instance.GetUrlFromSysUri("sys://Home"), Thread.CurrentThread.ManagedThreadId);
            //Server = new PremiseServer("home.kindel.net", 86, false, "premise", "0522");
            PremiseServer.Instance.ConnectComplete += Server_ConnectComplete;
            InitServerObjects();
            PageVisible = Visibility.Visible;

        }

        /// <summary>
        /// Update is called whenever the view for this vm needs data
        /// </summary>
        public void Update()
        {
            Debug.WriteLine("MainViewModel.Update. Thread = {0:X}", Thread.CurrentThread.ManagedThreadId);
            PremiseServer.Instance.Connect();
            //PremiseServer.Instance.QueryObects(objectList);
        }



        /// <summary>
        /// Event handler for when the server has completed a request.
        /// </summary>
        private void Server_ConnectComplete(Object s, PremiseServer.ConnectCompleteEventArgs ea)
        {
            Debug.WriteLine("ConnectComplete: {0} : {1} {2:X}", PremiseServer.Instance.GetUrlFromSysUri("sys://Home"), ea.Response.ToString(), Thread.CurrentThread.ManagedThreadId);
            DispatcherHelper.CheckBeginInvokeOnUI(() =>
            {
                if (ea.Succeeded)
                {
                    ApplicationTitle = ea.Response.ToString();
                    PageName = "Quick Access";
                    PremiseServer.Instance.QueryObects(objectList);     // Server.QueryAllObjects();
                    Visible = Visibility.Visible;
                }
                else
                {
                    ApplicationTitle = "No Connection";
                    PageName = ea.Response.ToString();
                }
            });
        }

        public const string ApplicationTitlePropertyName = "ApplicationTitle";
        private string _ApplicationTitle;
        /// <summary>
        /// ApplicationTitle property; this property is used in the view to display its value using a Binding
        /// </summary>
        /// <returns></returns>
        public string ApplicationTitle 
        {
            get
            {
                return _ApplicationTitle;
            }
            set
            {
                if (value != _ApplicationTitle)
                {
                    _ApplicationTitle = value;
                    RaisePropertyChanged(ApplicationTitlePropertyName);
                }
            }
        }

        public const string PageNamePropertyName = "PageName";
        private string _PageName;
        /// <summary>
        /// PageName property; this property is used in the view to display its value using a Binding
        /// </summary>
        /// <returns></returns>
        public string PageName
        {
            get
            {
                return _PageName;
            }
            set
            {
                if (value != _PageName)
                {
                    _PageName = value;
                    RaisePropertyChanged(PageNamePropertyName);
                }
            }
        }


        /// <summary>
        /// The <see cref="SelectedQuickViewItem" /> property's name.
        /// </summary>
        public const string SelectedQuickViewItemPropertyName = "SelectedQuickViewItem";

        private PremiseObject _SelectedQuickViewItem = null;

        /// <summary>
        /// Gets the SelectedQuickViewItem property: Used to handle ListBox selections.
        /// TODO Update documentation:
        /// Changes to that property's value raise the PropertyChanged event. 
        /// This property's value is broadcasted by the Messenger's default instance when it changes.
        /// </summary>
        public PremiseObject SelectedQuickViewItem
        {
            get
            {
                return _SelectedQuickViewItem;
            }

            set
            {
                if (_SelectedQuickViewItem == value)
                {
                    return;
                }

                var oldValue = _SelectedQuickViewItem;
                _SelectedQuickViewItem = value;

                if (_SelectedQuickViewItem != null && _SelectedQuickViewItem.Page != "")
                    Messenger.Default.Send<MainPageNavigateMessage>(new MainPageNavigateMessage(_SelectedQuickViewItem.Page));
            }
        }

        /// <summary>
        /// The <see cref="SelectedSettingsItem" /> property's name.
        /// </summary>
        public const string SelectedSettingsItemPropertyName = "SelectedSettingsItem";

        private SettingsViewModelBase _SelectedSettingsItem = null;

        /// <summary>
        /// Gets the SelectedSettingsItem property: Used to handle ListBox selections.
        /// TODO Update documentation:
        /// Changes to that property's value raise the PropertyChanged event. 
        /// This property's value is broadcasted by the Messenger's default instance when it changes.
        /// </summary>
        public SettingsViewModelBase SelectedSettingsItem
        {
            get
            {
                return _SelectedSettingsItem;
            }

            set
            {
                if (_SelectedSettingsItem == value)
                {
                    return;
                }

                var oldValue = _SelectedSettingsItem;
                _SelectedSettingsItem = value;

                if (_SelectedSettingsItem != null && _SelectedSettingsItem.Page != "")
                    Messenger.Default.Send<MainPageNavigateMessage>(new MainPageNavigateMessage(_SelectedSettingsItem.Page));
            }
        }

        /// <summary>
        /// The <see cref="Visible" /> property's name.
        /// </summary>
        public const string VisiblePropertyName = "Visible";

        private Visibility _Visibile = Visibility.Collapsed;

        /// <summary>
        /// Gets the Visible property.
        /// Visible if data has been loaded from server.  Collapsed if not. 
        /// Changes to that property's value raise the PropertyChanged event. 
        /// This property's value is broadcasted by the Messenger's default instance when it changes.
        /// </summary>
        public Visibility Visible
        {
            get
            {
                return _Visibile;
            }

            set
            {
                if (_Visibile == value)
                {
                    return;
                }

                var oldValue = _Visibile;
                _Visibile = value;
                // Update bindings, no sendToServer
                RaisePropertyChanged(VisiblePropertyName);
            }
        }

        /// <summary>
        /// The <see cref="PageVisible" /> property's name.
        /// </summary>
        public const string PageVisiblePropertyName = "PageVisible";

        private Visibility _PageVisible = Visibility.Collapsed;

        /// <summary>
        /// Gets the PageVisible property.
        /// PageVisible if data has been loaded from server.  Collapsed if not. 
        /// Changes to that property's value raise the PropertyChanged event. 
        /// This property's value is broadcasted by the Messenger's default instance when it changes.
        /// </summary>
        public Visibility PageVisible
        {
            get
            {
                return _PageVisible;
            }

            set
            {
                if (_PageVisible == value)
                {
                    return;
                }

                var oldValue = _PageVisible;
                _PageVisible = value;
                // Update bindings, no sendToServer
                RaisePropertyChanged(PageVisiblePropertyName);
            }
        }

        /// <summary>
        /// The <see cref="ProgressIndicatorVisible" /> property's name.
        /// </summary>
        public const string ProgressIndicatorVisiblePropertyName = "ProgressIndicatorVisible";

        private Visibility _ProgressIndicatorVisible = Visibility.Collapsed ;

        /// <summary>
        /// Gets the ProgressIndicatorVisible property.
        /// TODO Update documentation:
        /// Changes to that property's value raise the PropertyChanged event. 
        /// This property's value is broadcasted by the Messenger's default instance when it changes.
        /// </summary>
        public Visibility ProgressIndicatorVisible
        {
            get
            {
                return _ProgressIndicatorVisible;
            }

            set
            {
                if (_ProgressIndicatorVisible == value)
                {
                    return;
                }

                var oldValue = _ProgressIndicatorVisible;
                _ProgressIndicatorVisible = value;

                // Update bindings, no broadcast
                DispatcherHelper.UIDispatcher.BeginInvoke(() => RaisePropertyChanged(ProgressIndicatorVisiblePropertyName));
            }
        }


        /// <summary>
        /// A collection for Quick Access objects.
        /// </summary>
        public ObservableCollection<PremiseObject> QuickItems { get; private set; }

        /// <summary>
        /// Creates and adds a few GarageDoorSensor objects into the Server object.
        /// </summary>
        public void InitServerObjects()
        {
            Debug.WriteLine("MainViewModel.InitServerObjects");

            #region InitQuickItems
            this.QuickItems = new ObservableCollection<PremiseObject>();

            PremiseServer Server = PremiseServer.Instance;

            QuickItems.Add((PremiseObject)Server.GetOrCreateServerObject("PremiseObject",       "sys://Home/Media Zones",           "Audio/Video",      "Not implemented"));

            QuickItems.Add((PremiseObject)Server.GetOrCreateServerObject("SecurityQuickItem",   "sys://Home/Security",              "Security System",  "Arm and disarm the security system.", "/View/SecurityPanelPage.xaml"));

            QuickItems.Add((PremiseObject)Server.GetOrCreateServerObject("GDOQuickItem", "sys://Home", "Garage Doors", "Control Garage Doors", "/View/GarageDoorsPage.xaml"));

            QuickItems.Add((PremiseObject)Server.GetOrCreateServerObject("PremiseObject", "sys://Home/Admin/All House Scenes", "Lighting Scenes", "Control all house lighting scenes", "/View/AllHouseLightingPage.xaml"));

            QuickItems.Add((PremiseObject)Server.GetOrCreateServerObject("PremiseObject", "sys://Home/Upstairs/Upstairs Thermostat", "Heating & Cooling",   "Not implemented"));

            PremiseObject po = (PremiseObject)Server.GetOrCreateServerObject("IrrigationSystem", "sys://Home/Yard/IrrigationSystem", "Irrigation System", "Control the irrigation system.", "/View/IrrigationPage.xaml");
            QuickItems.Add(po);

            // Gotta keep track of all PremiseObjects on this view so they get updated
            // by the timer.
            foreach (PremiseObject item in QuickItems)
                objectList.Add((PremiseObject)item);
            #endregion

            // Initialize other ViewModels after the main view model (if needed)
            ViewModelLocator.AllHouseLightingViewModelStatic.InitServerObjects();
            //ViewModelLocator.GarageDoorSensorViewModelStatic.InitServerObjects();
            ViewModelLocator.GarageDoorsViewModelStatic.InitServerObjects();
            //ViewModelLocator.ItemViewModelStatic.InitServerObjects();
            ViewModelLocator.SecurityPanelViewModelStatic.InitServerObjects();
            ViewModelLocator.IrrigationViewModelStatic.InitServerObjects();
            //ViewModelLocator.SettingsViewModelBaseStatic.InitServerObjects();
        }

    }
    
}