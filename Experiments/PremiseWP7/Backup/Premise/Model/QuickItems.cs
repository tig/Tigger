using System.Collections.Generic;
using System;
using System.Windows.Threading;
using System.Windows.Data;
using System.Windows.Controls;
using System.Windows.Media.Imaging;
using System.Diagnostics;
using System.Reflection;
using GalaSoft.MvvmLight.Threading;
using Premise;
using Microsoft.Phone.Reactive;


namespace Premise.Model
{
    #region SecurityQuickItem
    public class SecurityQuickItem : PremiseObject
    {
        public static new string PremiseType = "SecurityQuickItem";
        public SecurityQuickItem(String location)
            : base(location)
        {
            // Override the base class's Value property to be PowerState
            _Value.PropertyName = "SecurityReady";
            _DisplayName.Persistent = true;

        }

        public override string FormatValue(object value)
        {
            return string.Format("{0}; {1}", this.SecurityState, this.SecurityReady ? "Ready" : "Not Ready");
        }

        enum SecuritySystemStates
        {
            Armed = 0,
            Disarmed = 1,
            StayArmed = 2,
            Alarming = 3,
            Disconnected = 4,
            Invalid = -1
        }

        static List<String> arStateNames = new List<string>
        {
            {"System Armed (Away)"},
            {"System Disarmed"},
            {"System Armed (Stay)"},
            {"Alarm Arming - Exit Delay"},
            {"Disconnected"},
            {"Unknown system state"}
        };

        private String SecuritySystemStateToString(SecuritySystemStates s)
        {
            if ((int)s < arStateNames.Count)
                return arStateNames[(int)s];
            else
                return arStateNames[(int)SecuritySystemStates.Invalid];
        }

        private SecuritySystemStates SecuritySystemStringToState(string str)
        {
            for (int i = 0; i < arStateNames.Count; i++)
			{
                if (arStateNames[i].CompareTo(str) == 0)
                    return (SecuritySystemStates)i;
			}
            return SecuritySystemStates.Invalid;
        }

        /// <summary>
        /// The <see cref="SecurityState" /> property's name.
        /// </summary>
        protected PremiseProperty<int> _SecurityState = new PremiseProperty<int>("SecurityState");
        /// <summary>
        /// Gets the SecurityState property.
        /// TODO Update documentation:
        /// Changes to that property's value raise the PropertyChanged event. 
        /// This property's value is broadcasted by the Messenger's default instance when it changes.
        /// </summary>
        // [PremiseProperty]
        public String SecurityState
        {
            get
            {
                return SecuritySystemStateToString((SecuritySystemStates)_SecurityState.Value);
            }

            set
            {
                var oldValue = _SecurityState.Value;
                _SecurityState.Value = (int) SecuritySystemStringToState(value);
                base.Value = value;

                // Raise change notification for both SecurityReady and Value because
                // Value is dependent on this.
                RaisePropertyChanged("Value", oldValue.ToString(), value, false);
                RaisePropertyChanged(_SecurityReady.PropertyName, oldValue.ToString(), (string)value, false);
            }
        }



        private void SecurityState_Get(PremiseServer server)
        {
            String propertyName = _SecurityState.PropertyName;

            server.GetPropertyAsync(Location, "SecurityState", (HttpResponseArgs) =>
            {
                if (!HttpResponseArgs.Succeeded)
                    return;

                //Debug.WriteLine("Received {0}: {1} = {2}", DisplayName, propertyName, HttpResponseArgs.Response);
                Int32 i;
                if (Int32.TryParse(HttpResponseArgs.Response, out i))
                {
                    this._SecurityState.UpdatedFromServer = false;
                    GalaSoft.MvvmLight.Threading.DispatcherHelper.CheckBeginInvokeOnUI(() =>
                    {
                        this.SecurityState = SecuritySystemStateToString((SecuritySystemStates)i);
                        this._SecurityState.UpdatedFromServer = true;
                        this.HasRealData = true;
                    });
                }
            });
        }


        /// <summary>
        /// The <see cref="SecurityReady" /> property's name.
        /// </summary>
        protected PremiseProperty<bool> _SecurityReady = new PremiseProperty<bool>("SecurityReady");
        /// <summary>
        /// Gets the SecurityReady property.
        /// TODO Update documentation:
        /// Changes to that property's value raise the PropertyChanged event. 
        /// This property's value is broadcasted by the Messenger's default instance when it changes.
        /// </summary>
        // [PremiseProperty]
        public bool SecurityReady
        {
            get { return _SecurityReady.Value;}

            set
            {
                var oldValue = _SecurityReady.Value;
                _SecurityReady.Value = value;

                // Raise change notification for both SecurityReady and Value because
                // Value is dependent on this.
                RaisePropertyChanged("Value", oldValue, value, false);
                RaisePropertyChanged(_SecurityReady.PropertyName, oldValue, value, false);
            }
        }

        private void SecurityReady_Get(PremiseServer server)
        {
            Debug.WriteLine("SecurityReady_Get");
            String propertyName = _SecurityReady.PropertyName;

            server.GetPropertyAsync(Location, "SecurityReady", (HttpResponseArgs) =>
            {
                if (!HttpResponseArgs.Succeeded)
                    return;

                //Debug.WriteLine("Received {0}: {1} = {2}", DisplayName, propertyName, HttpResponseArgs.Response);
                bool b;
                if (Boolean.TryParse(HttpResponseArgs.Response, out b))
                {
                    this._SecurityReady.UpdatedFromServer = false;
                    GalaSoft.MvvmLight.Threading.DispatcherHelper.CheckBeginInvokeOnUI(() =>
                    {
                        this.SecurityReady = b;
                        this._SecurityReady.UpdatedFromServer = true;
                        this.HasRealData = true;
                    });
                }
            });
        }

        /// <summary>
        /// The <see cref="NotReadyZones" /> property's name.
        /// </summary>
        private PremiseProperty<String> _NotReadyZones = new PremiseProperty<String>("NotReadyZones");

        /// <summary>
        /// Gets the NotReadyZones property.
        /// TODO Update documentation:
        /// Changes to that property's value raise the PropertyChanged event. 
        /// This property's value is broadcasted by the Messenger's default instance when it changes.
        /// </summary>
        public String NotReadyZones
        {
            get
            {
                return _NotReadyZones.Value;
            }

            set
            {
                if (_NotReadyZones.Value == value)
                {
                    return;
                }

                var oldValue = _NotReadyZones;
                _NotReadyZones.Value = value;

                RaisePropertyChanged(_NotReadyZones.PropertyName, oldValue.ToString(), value, false);
            }
        }


        protected void NotReadyZones_Get(PremiseServer server)
        {
            String propertyName = _NotReadyZones.PropertyName;

            _NotReadyZones.UpdatedFromServer = false;
            server.GetPropertyAsync(Location, propertyName, (HttpResponseArgs) =>
            {
                if (HttpResponseArgs.Succeeded)
                {
                    //Debug.WriteLine("Received {0}: {1} = {2}", NotReadyZones, propertyName, HttpResponseArgs.Response);
                    DispatcherHelper.CheckBeginInvokeOnUI(() =>
                    {
                        NotReadyZones = (String)HttpResponseArgs.Response; // <-- this is the whole cause of this confusing architecture
                        _NotReadyZones.UpdatedFromServer = true;
                        HasRealData = true;
                    });
                }
            });
        }

        /// <summary>
        /// The <see cref="DisarmCode" /> property's name.
        /// </summary>
        private PremiseProperty<String> _DisarmCode = new PremiseProperty<String>("DisarmCode");

        /// <summary>
        /// The disarm code for the security system.
        /// 
        /// Setting this to anything but a zero-length string will cause the disarm method
        /// to be invoked as well, after a delay to ensure the security system received
        /// the code first (specific to CADDX?).
        /// </summary>
        public String DisarmCode
        {
            get
            {
                return _DisarmCode.Value;
            }

            set
            {
                if (_DisarmCode.Value == value)
                {
                    return;
                }

                var oldValue = _DisarmCode;
                _DisarmCode.Value = value;

                RaisePropertyChanged(_DisarmCode.PropertyName, oldValue.ToString(), value, true);

                if (value.Length > 0)
                {
                    // Using Reactive to delay the send by 1/2 second to ensure
                    // the Code gets to the server first
                    Observable
                      .Timer(TimeSpan.FromMilliseconds(500))
                      .SubscribeOnDispatcher()
                      .Subscribe(_ =>
                      {
                          GalaSoft.MvvmLight.Threading.DispatcherHelper.CheckBeginInvokeOnUI(() => Disarm = true);
                      });
                }

            }
        }


        /// <summary>
        /// The <see cref="StayArm" /> property's name.
        /// </summary>
        protected PremiseProperty<bool> _StayArm = new PremiseProperty<bool>("StayArm");

        /// <summary>
        /// Gets the StayArm property.
        /// TODO Update documentation:
        /// Changes to that property's value raise the PropertyChanged event. 
        /// This property's value is broadcasted by the Messenger's default instance when it changes.
        /// </summary>
        public bool StayArm
        {
            set
            {
                if (value == true)
                    RaisePropertyChanged(_StayArm.PropertyName, false, value, true);// _StayArm.UpdatedFromServer);
            }
        }

        /// <summary>
        /// The <see cref="ExitArm" /> property's name.
        /// </summary>
        protected PremiseProperty<bool> _ExitArm = new PremiseProperty<bool>("ExitArm");

        /// <summary>
        /// Gets the ExitArm property.
        /// TODO Update documentation:
        /// Changes to that property's value raise the PropertyChanged event. 
        /// This property's value is broadcasted by the Messenger's default instance when it changes.
        /// </summary>
        public bool ExitArm
        {
            set
            {
                if (value == true)
                    RaisePropertyChanged(_ExitArm.PropertyName, false, value, true);// ExitArm.UpdatedFromServer);
            }
        }

        /// <summary>
        /// The <see cref="Disarm" /> property's name.
        /// </summary>
        protected PremiseProperty<bool> _Disarm = new PremiseProperty<bool>("Disarm");

        /// <summary>
        /// Gets the Disarm property.
        /// TODO Update documentation:
        /// Changes to that property's value raise the PropertyChanged event. 
        /// This property's value is broadcasted by the Messenger's default instance when it changes.
        /// </summary>
        public bool Disarm
        {
            set
            {
                if (value == true)
                    RaisePropertyChanged(_Disarm.PropertyName, false, value, true);// _Disarm.UpdatedFromServer);
            }
        }

        /// <summary>
        /// The <see cref="VacationModePlayback" /> property's name.
        /// </summary>
        protected PremiseProperty<Boolean> _VacationModePlayback = new PremiseProperty<bool>("VacationModePlayback");

        /// <summary>
        /// </summary>
        public Boolean VacationModePlayback
        {
            get { return _VacationModePlayback.Value; }

            set
            {
                if (_VacationModePlayback.Value == value) return;
                var oldValue = _VacationModePlayback.Value;
                _VacationModePlayback.Value = value;
                RaisePropertyChanged(_VacationModePlayback.PropertyName, oldValue, value, _VacationModePlayback.UpdatedFromServer);
            }
        }

        private void VacationModePlayback_Get(PremiseServer server)
        {
            String propertyName = _VacationModePlayback.PropertyName;
            server.GetPropertyAsync(Location, propertyName, (HttpResponseArgs) =>
            {
                if (HttpResponseArgs.Succeeded)
                {
                    //Debug.WriteLine("Received {0}: {1} = {2}", DisplayName, propertyName, HttpResponseArgs.Response);
                    bool b;
                    if (Boolean.TryParse(HttpResponseArgs.Response, out b))
                    {
                        _VacationModePlayback.UpdatedFromServer = false;
                        DispatcherHelper.CheckBeginInvokeOnUI(() =>
                        {
                            VacationModePlayback = b;
                            _VacationModePlayback.UpdatedFromServer = true;
                            HasRealData = true;
                        });
                    }
                }
            });
        }

        public override void QueryServer(PremiseServer server)
        {
            Debug.WriteLine("SecurityQuickItem.QueryServer");
            // Enumerate through each property 
            // Call base
            base.QueryServer(server);

            SecurityReady_Get(server);

            SecurityState_Get(server);

            NotReadyZones_Get(server);

            VacationModePlayback_Get(server);

            //DisarmCode_Get(server); // Don't bother ever getting from the server
        }
    }
    #endregion

    #region GDOQuickItem
    // QuickItem for the garage door system
    public class GDOQuickItem : PremiseObject
    {
        public static new string PremiseType = "GDOQuickItem";

        public GDOQuickItem(String location) : base(location)
        {
            // Override the base class's Value property to be PowerState
            _Value.PropertyName = "GarageDoorsClosed";
            _DisplayName.IgnoreServer = true;
        }

        public override String FormatValue(object value)
        {
            if (value != null)
            {
                if (value.ToString() == "False")
                    return "A garage door is open.";
                else
                    return "All garage doors are closed";
            }
            return "";
        }
    }
    #endregion

    
}