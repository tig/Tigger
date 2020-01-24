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

    public class IrrigationViewModel : ViewModelBase
    {
        public PremiseServer Server;

        public IrrigationViewModel()
        {
            Debug.WriteLine("IrrigationViewModel");
            if (IsInDesignMode)
            {
                // Code runs in Blend --> create design time data.
            }
            else
            {
                // Code runs "for real": Connect to service, etc...

                // Create our server instance. It will not talk on the network until we tell it to (Server.Connect).
                //if (ViewModelLocator.MainViewModelStatic.Server == null)
                //    ViewModelLocator.MainViewModelStatic.Server = new PremiseServer("home.kindel.net", 80);

            }
        }

        public string PageName
        {
            get
            {
                return "Irrigation System";
            }

        }

        /// <summary>
        /// The <see cref="IrrigationSystem" /> property's name.
        /// </summary>
        public const string IrrigationSystemPropertyName = "IrrigationSystem";

        private IrrigationSystem _irrigationSystem = null;

        /// <summary>
        /// Gets the IrrigationSystem property.
        /// TODO Update documentation:
        /// Changes to that property's value raise the PropertyChanged event. 
        /// This property's value is broadcasted by the Messenger's default instance when it changes.
        /// </summary>
        public IrrigationSystem IrrigationSystem
        {
            get
            {
                return _irrigationSystem;
            }

            set
            {
                if (_irrigationSystem == value)
                {
                    return;
                }

                var oldValue = _irrigationSystem;
                _irrigationSystem = value;

                // Update bindings, no broadcast
                RaisePropertyChanged(IrrigationSystemPropertyName);
            }
        }

        /// <summary>
        /// The <see cref="RainSensor" /> property's name.
        /// </summary>
        public const string RainSensorPropertyName = "RainSensor";

        private RainSensor _RainSensor = null;

        /// <summary>
        /// Gets the RainSensor property.
        /// TODO Update documentation:
        /// Changes to that property's value raise the PropertyChanged event. 
        /// This property's value is broadcasted by the Messenger's default instance when it changes.
        /// </summary>
        public RainSensor RainSensor
        {
            get
            {
                return _RainSensor;
            }

            set
            {
                if (_RainSensor == value)
                {
                    return;
                }

                var oldValue = _RainSensor;
                _RainSensor = value;

                // Update bindings, no broadcast
                RaisePropertyChanged(RainSensorPropertyName);
            }
        }

        /// <summary>
        /// The <see cref="IrrigationPump" /> property's name.
        /// </summary>
        public const string IrrigationPumpPropertyName = "IrrigationPump";

        private IrrigationPump _IrrigationPump = null;

        /// <summary>
        /// Gets the IrrigationPump property.
        /// TODO Update documentation:
        /// Changes to that property's value raise the PropertyChanged event. 
        /// This property's value is broadcasted by the Messenger's default instance when it changes.
        /// </summary>
        public IrrigationPump IrrigationPump
        {
            get
            {
                return _IrrigationPump;
            }

            set
            {
                if (_IrrigationPump == value)
                {
                    return;
                }

                var oldValue = _IrrigationPump;
                _IrrigationPump = value;

                // Update bindings, no broadcast
                RaisePropertyChanged(IrrigationPumpPropertyName);
            }
        }

        /// <summary>
        /// Creates and adds a few GarageDoorSensor objects into the Server object.
        /// </summary>
        public void InitServerObjects()
        {
            Debug.WriteLine("IrrigationViewModel.InitServerObjects");

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
                        //ApplicationTitle = ea.Response.ToString();
                        //Server.QueryAllObjects();
                    });
                }
              //  else
              //      DispatcherHelper.CheckBeginInvokeOnUI(() => ApplicationTitle = "No Connection: " + ea.Response.ToString());
            };

            IrrigationSystem = ((IrrigationSystem)Server.GetOrCreateServerObject("IrrigationSystem", "sys://Home/Yard/IrrigationSystem"));
            RainSensor = ((RainSensor)Server.GetOrCreateServerObject("RainSensor", "sys://Home/Yard/IrrigationSystem/RainSensor"));
            IrrigationPump = ((IrrigationPump)Server.GetOrCreateServerObject("IrrigationPump", "sys://Home/Yard/IrrigationSystem/IrrigationPump"));
        }

        public void Update()
        {
            IrrigationSystem.QueryServer(Server);
            RainSensor.QueryServer(Server);
            IrrigationPump.QueryServer(Server);
        }
    }

    public class IrrigationSystemStateFormatConverter : IValueConverter
    {
        #region IValueConverter Members

        public object Convert(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
        {
            if (value != null && value.GetType() == typeof(bool))
            {
                if ((bool)value)
                    return "Running";
                else
                    return "Stopped";
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