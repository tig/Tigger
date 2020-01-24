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

namespace Premise.Model
{
    #region IrrigationSystem
    public class IrrigationSystem : PremiseObject
    {
        public static new string PremiseType = "IrrigationSystem";

        public IrrigationSystem(String location)
            : base(location)
        {
            // Override the base class's Value property to be SystemState
            _Value.PropertyName = "SystemState";
            _DisplayName.IgnoreServer = true;
        }

        public override String FormatValue(object value)
        {
            if (value != null)
            {
                if (value.ToString() == "False")
                    return "Stopped";
                else
                    return "Running";
            }
            return "";
        }

        /// <summary>
        /// The <see cref="SystemState" /> property's name.
        /// </summary>
        protected PremiseProperty<Boolean> _SystemState = new PremiseProperty<bool>("SystemState");

        /// <summary>
        /// </summary>
        public Boolean SystemState
        {
            get
            {
                return _SystemState.Value;
            }

            set
            {
                if (_SystemState.Value == value)
                {
                    return;
                }

                var oldValue = _SystemState.Value;
                _SystemState.Value = value;

                base.Value = value.ToString();
                RaisePropertyChanged(_SystemState.PropertyName, oldValue, value, _SystemState.UpdatedFromServer);
            }
        }

        private void SystemState_Get(PremiseServer server)
        {
            String propertyName = _SystemState.PropertyName;
            server.GetPropertyAsync(Location, "SystemState", (HttpResponseArgs) =>
            {
                if (!HttpResponseArgs.Succeeded)
                    return;

                //Debug.WriteLine("Received {0}: {1} = {2}", DisplayName, propertyName, HttpResponseArgs.Response);
                bool b;
                if (Boolean.TryParse(HttpResponseArgs.Response, out b))
                {
                    _SystemState.UpdatedFromServer = false;
                    DispatcherHelper.CheckBeginInvokeOnUI(() =>
                    {
                        SystemState = b;
                        _SystemState.UpdatedFromServer = true;
                        HasRealData = true;
                    });
                }
            });
        }

        /// <summary>
        /// The <see cref="RunSchedule" /> property's name.
        /// </summary>
        protected PremiseProperty<Boolean> _RunSchedule = new PremiseProperty<bool>("RunSchedule");

        /// <summary>
        /// </summary>
        public Boolean RunSchedule
        {
            get
            {
                return _RunSchedule.Value;
            }

            set
            {
                if (_RunSchedule.Value == value)
                {
                    return;
                }

                var oldValue = _RunSchedule.Value;
                _RunSchedule.Value = value;

                RaisePropertyChanged(_RunSchedule.PropertyName, oldValue, value, _RunSchedule.UpdatedFromServer);
            }
        }

        private void RunSchedule_Get(PremiseServer server)
        {
            String propertyName = _RunSchedule.PropertyName;
            server.GetPropertyAsync(Location, "RunSchedule", (HttpResponseArgs) =>
            {
                if (!HttpResponseArgs.Succeeded)
                    return;

                //Debug.WriteLine("Received {0}: {1} = {2}", DisplayName, propertyName, HttpResponseArgs.Response);
                bool b;
                if (Boolean.TryParse(HttpResponseArgs.Response, out b))
                {
                    _RunSchedule.UpdatedFromServer = false;
                    DispatcherHelper.CheckBeginInvokeOnUI(() =>
                    {
                        RunSchedule = b;
                        _RunSchedule.UpdatedFromServer = true;
                        HasRealData = true;
                    });
                }
            });
        }

        /// <summary>
        /// The <see cref="PumpEnabled" /> property's name.
        /// </summary>
        protected PremiseProperty<Boolean> _PumpEnabled = new PremiseProperty<bool>("PumpEnabled");

        /// <summary>
        /// </summary>
        public Boolean PumpEnabled
        {
            get
            {
                return _PumpEnabled.Value;
            }

            set
            {
                if (_PumpEnabled.Value == value)
                {
                    return;
                }

                var oldValue = _PumpEnabled.Value;
                _PumpEnabled.Value = value;

                RaisePropertyChanged(_PumpEnabled.PropertyName, oldValue, value, _PumpEnabled.UpdatedFromServer);
            }
        }

        private void PumpEnabled_Get(PremiseServer server)
        {
            String propertyName = _PumpEnabled.PropertyName;
            server.GetPropertyAsync(Location, "PumpEnabled", (HttpResponseArgs) =>
            {
                if (!HttpResponseArgs.Succeeded)
                    return;

                //Debug.WriteLine("Received {0}: {1} = {2}", DisplayName, propertyName, HttpResponseArgs.Response);
                bool b;
                if (Boolean.TryParse(HttpResponseArgs.Response, out b))
                {
                    _PumpEnabled.UpdatedFromServer = false;
                    DispatcherHelper.CheckBeginInvokeOnUI(() =>
                    {
                        PumpEnabled = b;
                        _PumpEnabled.UpdatedFromServer = true;
                        HasRealData = true;
                    });
                }
            });
        }

        /// <summary>
        /// The <see cref="Paused" /> property's name.
        /// </summary>
        protected PremiseProperty<Boolean> _Paused = new PremiseProperty<bool>("Paused");

        /// <summary>
        /// </summary>
        public Boolean Paused
        {
            get {return _Paused.Value;}

            set
            {
                if (_Paused.Value == value) return;
                var oldValue = _Paused.Value;
                _Paused.Value = value;
                RaisePropertyChanged(_Paused.PropertyName, oldValue, value, _Paused.UpdatedFromServer);
            }
        }

        private void Paused_Get(PremiseServer server)
        {
            String propertyName = _Paused.PropertyName;
            server.GetPropertyAsync(Location, propertyName, (HttpResponseArgs) =>
            {
                if (HttpResponseArgs.Succeeded)
                {
                    //Debug.WriteLine("Received {0}: {1} = {2}", DisplayName, propertyName, HttpResponseArgs.Response);
                    bool b;
                    if (Boolean.TryParse(HttpResponseArgs.Response, out b))
                    {
                        _Paused.UpdatedFromServer = false;
                        DispatcherHelper.CheckBeginInvokeOnUI(() =>
                        {
                            Paused = b;
                            _Paused.UpdatedFromServer = true;
                            HasRealData = true;
                        });
                    }
                }
            });
        }

        /// <summary>
        /// The <see cref="RainSensorEnabled" /> property's name.
        /// </summary>
        protected PremiseProperty<Boolean> _RainSensorEnabled = new PremiseProperty<bool>("RainSensorEnabled");

        /// <summary>
        /// </summary>
        public Boolean RainSensorEnabled
        {
            get
            {
                return _RainSensorEnabled.Value;
            }

            set
            {
                if (_RainSensorEnabled.Value == value)
                {
                    return;
                }

                var oldValue = _RainSensorEnabled.Value;
                _RainSensorEnabled.Value = value;

                RaisePropertyChanged(_RainSensorEnabled.PropertyName, oldValue, value, _RainSensorEnabled.UpdatedFromServer);
            }
        }

        private void RainSensorEnabled_Get(PremiseServer server)
        {
            String propertyName = _RainSensorEnabled.PropertyName;
            server.GetPropertyAsync(Location, "RainSensorEnabled", (HttpResponseArgs) =>
            {
                if (!HttpResponseArgs.Succeeded)
                    return;

                //Debug.WriteLine("Received {0}: {1} = {2}", DisplayName, propertyName, HttpResponseArgs.Response);
                bool b;
                if (Boolean.TryParse(HttpResponseArgs.Response, out b))
                {
                    _RainSensorEnabled.UpdatedFromServer = false;
                    DispatcherHelper.CheckBeginInvokeOnUI(() =>
                    {
                        RainSensorEnabled = b;
                        _RainSensorEnabled.UpdatedFromServer = true;
                        HasRealData = true;
                    });
                }
            });
        }

        /// <summary>
        /// The <see cref="TestRunAll" /> property's name.
        /// </summary>
        protected PremiseProperty<Boolean> _TestRunAll = new PremiseProperty<bool>("TestRunAll");

        /// <summary>
        /// </summary>
        public Boolean TestRunAll
        {
            get
            {
                return _TestRunAll.Value;
            }

            set
            {
                if (_TestRunAll.Value == value)
                {
                    return;
                }

                var oldValue = _TestRunAll.Value;
                _TestRunAll.Value = value;

                RaisePropertyChanged(_TestRunAll.PropertyName, oldValue, value, _TestRunAll.UpdatedFromServer);
            }
        }

        private void TestRunAll_Get(PremiseServer server)
        {
            String propertyName = _TestRunAll.PropertyName;
            server.GetPropertyAsync(Location, "TestRunAll", (HttpResponseArgs) =>
            {
                if (!HttpResponseArgs.Succeeded)
                    return;

                //Debug.WriteLine("Received {0}: {1} = {2}", DisplayName, propertyName, HttpResponseArgs.Response);
                bool b;
                if (Boolean.TryParse(HttpResponseArgs.Response, out b))
                {
                    _TestRunAll.UpdatedFromServer = false;
                    DispatcherHelper.CheckBeginInvokeOnUI(() =>
                    {
                        TestRunAll = b;
                        _TestRunAll.UpdatedFromServer = true;
                        HasRealData = true;
                    });
                }
            });
        }

        /// <summary>
        /// The <see cref="SystemMode" /> property's name.
        /// </summary>
        protected PremiseProperty<String> _SystemMode = new PremiseProperty<String>("SystemMode");

        /// <summary>
        /// </summary>
        public String SystemMode
        {
            get
            {
                return _SystemMode.Value;
            }

            set
            {
                if (_SystemMode.Value == value)
                {
                    return;
                }

                var oldValue = _SystemMode.Value;
                _SystemMode.Value = value;

                RaisePropertyChanged(_SystemMode.PropertyName, oldValue, value, _SystemMode.UpdatedFromServer);
            }
        }

        private void SystemMode_Get(PremiseServer server)
        {
            String propertyName = _SystemMode.PropertyName;
            server.GetPropertyAsync(Location, "SystemMode", (HttpResponseArgs) =>
            {
                if (!HttpResponseArgs.Succeeded)
                    return;

                //Debug.WriteLine("Received {0}: {1} = {2}", DisplayName, propertyName, HttpResponseArgs.Response);
                _SystemMode.UpdatedFromServer = false;
                DispatcherHelper.CheckBeginInvokeOnUI(() =>
                {
                    SystemMode = HttpResponseArgs.Response;  // TODO: This is an enum value
                    _SystemMode.UpdatedFromServer = true;
                    HasRealData = true;
                });
            });
        }

        /// <summary>
        /// The <see cref="RunState" /> property's name.
        /// </summary>
        protected PremiseProperty<String> _RunState = new PremiseProperty<String>("RunState");

        /// <summary>
        /// </summary>
        public String RunState
        {
            get
            {
                return _RunState.Value;
            }

            set
            {
                if (_RunState.Value == value)
                {
                    return;
                }

                var oldValue = _RunState.Value;
                _RunState.Value = value;

                RaisePropertyChanged(_RunState.PropertyName, oldValue, value, _RunState.UpdatedFromServer);
            }
        }

        private void RunState_Get(PremiseServer server)
        {
            String propertyName = _RunState.PropertyName;
            server.GetPropertyAsync(Location, "RunState", (HttpResponseArgs) =>
            {
                if (!HttpResponseArgs.Succeeded)
                    return;

                //Debug.WriteLine("Received {0}: {1} = {2}", DisplayName, propertyName, HttpResponseArgs.Response);
                _RunState.UpdatedFromServer = false;
                DispatcherHelper.CheckBeginInvokeOnUI(() =>
                {
                    RunState = HttpResponseArgs.Response;  // TODO: This is an enum value
                    _RunState.UpdatedFromServer = true;
                    HasRealData = true;
                });
            });
        }


        /// <summary>
        /// The <see cref="LastRun" /> property's name.
        /// </summary>
        protected PremiseProperty<DateTime> _LastRun = new PremiseProperty<DateTime>("LastRun");

        /// <summary>
        /// </summary>
        public DateTime LastRun
        {
            get { return _LastRun.Value;}

            set
            {
                if (_LastRun.Value == value) return;
                var oldValue = _LastRun.Value;
                _LastRun.Value = value;
                RaisePropertyChanged(_LastRun.PropertyName, oldValue, value, _LastRun.UpdatedFromServer);
            }
        }

        private void LastRun_Get(PremiseServer server)
        {
            String propertyName = _LastRun.PropertyName;
            server.GetPropertyAsync(Location, propertyName, (HttpResponseArgs) =>
            {
                if (HttpResponseArgs.Succeeded)
                {
                    //Debug.WriteLine("Received {0}: {1} = {2}", DisplayName, propertyName, HttpResponseArgs.Response);
                    _LastRun.UpdatedFromServer = false;
                    DispatcherHelper.CheckBeginInvokeOnUI(() =>
                    {
                        LastRun = DateTime.Parse(HttpResponseArgs.Response);  
                        _LastRun.UpdatedFromServer = true;
                        HasRealData = true;
                    });
                }
            });
        }

        /// <summary>
        /// The <see cref="StartTime" /> property's name.
        /// </summary>
        protected PremiseProperty<DateTime> _StartTime = new PremiseProperty<DateTime>("StartTime");

        /// <summary>
        /// </summary>
        public DateTime StartTime
        {
            get { return _StartTime.Value; }

            set
            {
                if (_StartTime.Value == value) return;
                var oldValue = _StartTime.Value;
                _StartTime.Value = value;
                RaisePropertyChanged(_StartTime.PropertyName, oldValue, value, _StartTime.UpdatedFromServer);
            }
        }

        private void StartTime_Get(PremiseServer server)
        {
            String propertyName = _StartTime.PropertyName;
            server.GetPropertyAsync(Location, propertyName, (HttpResponseArgs) =>
            {
                if (HttpResponseArgs.Succeeded)
                {
                    //Debug.WriteLine("Received {0}: {1} = {2}", DisplayName, propertyName, HttpResponseArgs.Response);
                    _StartTime.UpdatedFromServer = false;
                    DispatcherHelper.CheckBeginInvokeOnUI(() =>
                    {
                        StartTime = DateTime.Parse(HttpResponseArgs.Response);
                        _StartTime.UpdatedFromServer = true;
                        HasRealData = true;
                    });
                }
            });
        }

        /// <summary>
        /// The <see cref="TestRunTime" /> property's name.
        /// </summary>
        protected PremiseProperty<int> _TestRunTime = new PremiseProperty<int>("TestRunTime");

        /// <summary>
        /// </summary>
        public int TestRunTime
        {
            get { return _TestRunTime.Value; }

            set
            {
                if (_TestRunTime.Value == value) return;
                var oldValue = _TestRunTime.Value;
                _TestRunTime.Value = value;
                RaisePropertyChanged(_TestRunTime.PropertyName, oldValue, value, _TestRunTime.UpdatedFromServer);
            }
        }

        private void TestRunTime_Get(PremiseServer server)
        {
            String propertyName = _TestRunTime.PropertyName;
            server.GetPropertyAsync(Location, propertyName, (HttpResponseArgs) =>
            {
                if (HttpResponseArgs.Succeeded)
                {
                    //Debug.WriteLine("Received {0}: {1} = {2}", DisplayName, propertyName, HttpResponseArgs.Response);
                    _TestRunTime.UpdatedFromServer = false;
                    DispatcherHelper.CheckBeginInvokeOnUI(() =>
                    {
                        TestRunTime = int.Parse(HttpResponseArgs.Response);
                        _TestRunTime.UpdatedFromServer = true;
                        HasRealData = true;
                    });
                }
            });
        }

        /// <summary>
        /// The <see cref="DelayStartTime" /> property's name.
        /// </summary>
        protected PremiseProperty<int> _DelayStartTime = new PremiseProperty<int>("DelayStartTime");

        /// <summary>
        /// </summary>
        public int DelayStartTime
        {
            get { return _DelayStartTime.Value; }

            set
            {
                if (_DelayStartTime.Value == value) return;
                var oldValue = _DelayStartTime.Value;
                _DelayStartTime.Value = value;
                RaisePropertyChanged(_DelayStartTime.PropertyName, oldValue, value, _DelayStartTime.UpdatedFromServer);
            }
        }

        private void DelayStartTime_Get(PremiseServer server)
        {
            String propertyName = _DelayStartTime.PropertyName;
            server.GetPropertyAsync(Location, propertyName, (HttpResponseArgs) =>
            {
                if (HttpResponseArgs.Succeeded)
                {
                    //Debug.WriteLine("Received {0}: {1} = {2}", DisplayName, propertyName, HttpResponseArgs.Response);
                    _DelayStartTime.UpdatedFromServer = false;
                    DispatcherHelper.CheckBeginInvokeOnUI(() =>
                    {
                        DelayStartTime = int.Parse(HttpResponseArgs.Response);
                        _DelayStartTime.UpdatedFromServer = true;
                        HasRealData = true;
                    });
                }
            });
        }

        /// <summary>
        /// The <see cref="SystemPctRunTime" /> property's name.
        /// </summary>
        protected PremiseProperty<int> _SystemPctRunTime = new PremiseProperty<int>("SystemPctRunTime");

        /// <summary>
        /// </summary>
        public int SystemPctRunTime
        {
            get { return _SystemPctRunTime.Value; }

            set
            {
                if (_SystemPctRunTime.Value == value) return;
                var oldValue = _SystemPctRunTime.Value;
                _SystemPctRunTime.Value = value;
                RaisePropertyChanged(_SystemPctRunTime.PropertyName, oldValue, value, _SystemPctRunTime.UpdatedFromServer);
            }
        }

        private void SystemPctRunTime_Get(PremiseServer server)
        {
            String propertyName = _SystemPctRunTime.PropertyName;
            server.GetPropertyAsync(Location, propertyName, (HttpResponseArgs) =>
            {
                if (HttpResponseArgs.Succeeded)
                {
                    //Debug.WriteLine("Received {0}: {1} = {2}", DisplayName, propertyName, HttpResponseArgs.Response);
                    _SystemPctRunTime.UpdatedFromServer = false;
                    DispatcherHelper.CheckBeginInvokeOnUI(() =>
                    {
                        SystemPctRunTime = int.Parse(HttpResponseArgs.Response);
                        _SystemPctRunTime.UpdatedFromServer = true;
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

            SystemState_Get(server);
            RunSchedule_Get(server);
            PumpEnabled_Get(server);
            Paused_Get(server);
            RainSensorEnabled_Get(server);
            TestRunAll_Get(server);
            SystemMode_Get(server);
            RunState_Get(server);
            LastRun_Get(server);
            StartTime_Get(server);
            TestRunTime_Get(server);
            DelayStartTime_Get(server);
            SystemPctRunTime_Get(server);
        }
    }
    #endregion

    #region RainSensor
    public class RainSensor : PremiseObject
    {
        public static new string PremiseType = "RainSensor";

        public RainSensor(String location)
            : base(location)
        {
            // Override the base class's Value property to be SystemState
            _Value.PropertyName = "WaterDetected";
            _DisplayName.IgnoreServer = true;
        }

    }
    #endregion

    #region IrrigationPump
    public class IrrigationPump : PremiseObject
    {
        public static new string PremiseType = "IrrigationPump";

        public IrrigationPump(String location)
            : base(location)
        {
            // Override the base class's Value property to be SystemState
            _Value.PropertyName = "State";
            _DisplayName.IgnoreServer = true;
        }

        /// <summary>
        /// The <see cref="State" /> property's name.
        /// </summary>
        protected PremiseProperty<Boolean> _State = new PremiseProperty<bool>("State");

        /// <summary>
        /// </summary>
        public Boolean State
        {
            get { return _State.Value; }

            set
            {
                if (_State.Value == value) return;
                var oldValue = _State.Value;
                _State.Value = value;
                RaisePropertyChanged(_State.PropertyName, oldValue, value, _State.UpdatedFromServer);
            }
        }

        private void State_Get(PremiseServer server)
        {
            String propertyName = _State.PropertyName;
            server.GetPropertyAsync(Location, propertyName, (HttpResponseArgs) =>
            {
                if (HttpResponseArgs.Succeeded)
                {
                    //Debug.WriteLine("Received {0}: {1} = {2}", DisplayName, propertyName, HttpResponseArgs.Response);
                    bool b;
                    if (Boolean.TryParse(HttpResponseArgs.Response, out b))
                    {
                        _State.UpdatedFromServer = false;
                        DispatcherHelper.CheckBeginInvokeOnUI(() =>
                        {
                            State = b;
                            _State.UpdatedFromServer = true;
                            HasRealData = true;
                        });
                    }
                }
            });
        }

        public override void QueryServer(PremiseServer server)
        {
            // Enumerate through each property 

            // Call base
            base.QueryServer(server);
            State_Get(server);
        }

    }
    #endregion

}
