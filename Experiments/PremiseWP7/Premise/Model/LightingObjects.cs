using System;
using System.Net;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Documents;
using System.Windows.Data;
using System.Windows.Input;
using System.Diagnostics;
using System.Windows.Media.Animation;
using System.Windows.Shapes;
using GalaSoft.MvvmLight;
using GalaSoft.MvvmLight.Messaging;
using GalaSoft.MvvmLight.Threading;
using GalaSoft.MvvmLight.Command;


namespace Premise.Model
{
    #region Light
    public class Light : PremiseObject
    {
        public static new string PremiseType = "Light";
        public Light(String location) 
            : base(location)
        {
            // Override the base class's Value property to be PowerState
            _Value.PropertyName = "PowerState";
            _DisplayName.Persistent = true;
        }

        public override String FormatValue(object value)
        {
            if (value != null)
            {
                if (value.ToString() == "True")
                    return "On";
                else
                    return "Off";
            }
            return "Error";
        }

        /// <summary>
        /// The <see cref="PowerState" /> property's name.
        /// </summary>
        protected PremiseProperty<Boolean> _PowerState = new PremiseProperty<bool>("PowerState");

        /// <summary>
        /// Gets the PowerState property.
        /// TODO Update documentation:
        /// Changes to that property's value raise the PropertyChanged event. 
        /// This property's value is broadcasted by the Messenger's default instance when it changes.
        /// </summary>
        // [PremiseProperty]
        public Boolean PowerState
        {
            get
            {
                return _PowerState.Value;
            }

            set
            {
                //if (_PowerState.Value == value)
                //{
                //                    return;
                //              }

                var oldValue = _PowerState.Value;
                _PowerState.Value = value;

                base.Value = value.ToString();

                RaisePropertyChanged(_PowerState.PropertyName, oldValue, value, _PowerState.UpdatedFromServer);
            }
        }

        private void PowerState_Get(PremiseServer server)
        {
            String propertyName = _PowerState.PropertyName;
            server.GetPropertyAsync(Location, "PowerState", (HttpResponseArgs) =>
            {
                if (!HttpResponseArgs.Succeeded)
                    return;

                //Debug.WriteLine("Received {0}: {1} = {2}", DisplayName, propertyName, HttpResponseArgs.Response);
                bool b;
                if (Boolean.TryParse(HttpResponseArgs.Response, out b))
                {
                    _PowerState.UpdatedFromServer = false;
                    DispatcherHelper.CheckBeginInvokeOnUI(() =>
                    {
                        PowerState = b;
                        _PowerState.UpdatedFromServer = true;
                        HasRealData = true;
                    });
                }
            });
        }
        
        public override void QueryServer(PremiseServer server)
        {
            // Enumerate through each property 

            // Call base
            base.QueryServer(server);

            PowerState_Get(server);

        }


    }
    #endregion

    #region LightingScene
    public class LightingScene : PremiseObject
    {
        public static new string PremiseType = "LightingScene";
        public LightingScene(String location)
            : base(location)
        {
            // Override the base class's Value property to be PowerState
            _Value.PropertyName = "Status";
            _DisplayName.Persistent = true;

            TriggerCommand = new RelayCommand<String>((s) =>
            {
                Debug.WriteLine("TriggerCommand {0}", s);
                Trigger = true; // Execute (Action)
            });

        }

        public LightingScene()
            : this("")
        {
            // for design time
        }

        // Commands
        #region Command properties
        public RelayCommand<String> TriggerCommand { get; private set; }
        #endregion

        public override String FormatValue(object value)
        {
            if (value != null)
            {
                if (value.ToString() == "True")
                    return "On";
                else
                    return "Off";
            }
            return "Error";
        }


        /// <summary>
        /// The <see cref="Trigger" /> property's name.
        /// </summary>
        protected PremiseProperty<bool> _Trigger = new PremiseProperty<bool>("Trigger");

        /// <summary>
        /// Gets the Trigger property.
        /// TODO Update documentation:
        /// Changes to that property's value raise the PropertyChanged event. 
        /// This property's value is broadcasted by the Messenger's default instance when it changes.
        /// </summary>
        public bool Trigger
        {
            set
            {
                if (value == true)
                    RaisePropertyChanged(_Trigger.PropertyName, false, value, true);// _Trigger.UpdatedFromServer);
            }
        }


        public override void QueryServer(PremiseServer server)
        {
            // Enumerate through each property 

            // Call base
            base.QueryServer(server); 
        }


    }
    public class LightingStatusFormatConverter : IValueConverter
    {
        #region IValueConverter Members

        public object Convert(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
        {
            if (value != null && value.GetType() == typeof(bool))
            {
                if ((bool)value)
                    return "On";
                else
                    return "Off";
            }
            return "Error";
        }

        public object ConvertBack(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
        {
            throw new NotImplementedException();
        }

        #endregion
    }


    #endregion

    #region LightingScenes
    public class LightingScenes : PremiseObject
    {
        public static new string PremiseType = "LightingScenes";
        public LightingScenes(String location)
            : base(location)
        {
            // Override the base class's Value property to be PowerState
            //_Value.PropertyName = "Description";
        }

        public override void QueryServer(PremiseServer server)
        {
            // Enumerate through each property 

            // Call base
            base.QueryServer(server);
        }

    }
    #endregion
}
