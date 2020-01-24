using System;
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
using GalaSoft.MvvmLight.Command;
using GalaSoft.MvvmLight.Threading;
using System.Reflection;
using Premise;
using Premise.Model;

namespace Premise.ViewModel
{

    public class GarageDoorsViewModel : ViewModelBase
    {
        public PremiseServer Server;

        public GarageDoorsViewModel()
        {
            Debug.WriteLine("GarageDoorsViewModel");
            if (IsInDesignMode)
            {
                // Code runs in Blend --> create design time data.
            }
            else
            {
                // Code runs "for real": Connect to service, etc...

                ApplicationTitle = "";

                // Create our server instance. It will not talk on the network until we tell it to (Server.Connect).
                //if (ViewModelLocator.MainViewModelStatic.Server == null)
                //    ViewModelLocator.MainViewModelStatic.Server = new PremiseServer("home.kindel.net", 80);

            }
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

        public string PageName
        {
            get
            {
                return "Garage Doors";
            }

        }

       
        /// <summary>
        /// A collection for GarageDoorSensor objects.
        /// </summary>
        public ObservableCollection<GarageDoorSensor> Items { get; private set; }
        /// <summary>
        /// Creates and adds a few GarageDoorSensor objects into the Server object.
        /// </summary>
        public void InitServerObjects()
        {
            Debug.WriteLine("GarageDoorsViewModel.InitServerObjects");

            Server = PremiseServer.Instance;

            // Invoke connect async method. When our delegate is called the connection attempt 
            // is complete.  It's either a success (Response is the DisplayName of Home) or a failure 
            // (Response is the error).
            //
            Server.ConnectComplete += (o, ea) =>
            {
                if (ea.Succeeded)
                {
                    GalaSoft.MvvmLight.Threading.DispatcherHelper.CheckBeginInvokeOnUI(() =>
                    {
                        ApplicationTitle = ea.Response.ToString();
                        //Server.QueryAllObjects();
                    });
                }
                else
                    DispatcherHelper.CheckBeginInvokeOnUI(() => ApplicationTitle = "No Connection: " + ea.Response.ToString());
            };

            Items = new ObservableCollection<GarageDoorSensor>();
            Items.Add((GarageDoorSensor)Server.GetOrCreateServerObject("GarageDoorSensor", "sys://Home/Upstairs/Garage/West Garage Door"));
            Items.Add((GarageDoorSensor)Server.GetOrCreateServerObject("GarageDoorSensor", "sys://Home/Upstairs/Garage/East Garage Door"));
            Items.Add((GarageDoorSensor)Server.GetOrCreateServerObject("GarageDoorSensor", "sys://Home/Upper Garage/West Garage Door"));
            Items.Add((GarageDoorSensor)Server.GetOrCreateServerObject("GarageDoorSensor", "sys://Home/Upper Garage/Center Garage Door"));
            Items.Add((GarageDoorSensor)Server.GetOrCreateServerObject("GarageDoorSensor", "sys://Home/Upper Garage/East Garage Door"));
            // Gotta keep track of all PremiseObjects on this view so they get updated
            // by the timer.
            foreach (GarageDoorSensor item in Items)
            {
                objectList.Add((PremiseObject)item);
            }

        }

        private List<PremiseObject> objectList = new List<PremiseObject>();
        public void Update()
        {
            Server.QueryObects(objectList);
        }
    }

    public class GDOStateFormatConverter : IValueConverter
    {
        #region IValueConverter Members

        public object Convert(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
        {
            if (value != null && value.GetType() == typeof(bool))
            {
                if ((bool)value)
                    return "Open";
                else
                    return "Closed";
            }
            return "Error";
        }

        public object ConvertBack(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
        {
            throw new NotImplementedException();
        }

        #endregion
    }
}