using GalaSoft.MvvmLight;
using GalaSoft.MvvmLight.Command;
using GalaSoft.MvvmLight.Threading;
using System;
using System.ComponentModel;
using System.Diagnostics;
using Premise;

namespace Premise.Model
{
    public class GarageDoorSensor : PremiseObject
    {
        public static new string PremiseType = "GarageDoorSensor";
        public GarageDoorSensor(String location) : base(location)
        {
            // Override the base class's Value property to be GarageDoorStatus
            _Value.PropertyName = "GarageDoorStatus";

            _DisplayName.Persistent = true;

            TriggerCommand = new RelayCommand<String>((s) =>
                 {
                     Debug.WriteLine("TriggerCommand {0}", s);
                     Trigger = true; // Execute (Action)
                 });
        }

        // Commands
        #region Command properties
        public RelayCommand<String> TriggerCommand { get; private set; }
        #endregion

        public override String FormatValue(object value)
        {
            if (_Value.Value != null)
                return _Value.Value;
            else
                return "No value!";
            // Updates the name on each KeyUp event (which will re-evaluate the SaveCommand)

        }

        public override void QueryServer(PremiseServer server)
        {
            // Enumerate through each property 

            // Call base
            base.QueryServer(server);

            server.GetPropertyAsync(Location, "GarageDoorOpened", (HttpResponseArgs) =>
            {
                if (HttpResponseArgs.Succeeded)
                {
                    bool b;
                    if (Boolean.TryParse(HttpResponseArgs.Response, out b))
                    {
                        this._GarageDoorOpened.UpdatedFromServer = false;
                        GalaSoft.MvvmLight.Threading.DispatcherHelper.CheckBeginInvokeOnUI(() =>
                            {
                                this.GarageDoorOpened = b;
                                this._GarageDoorOpened.UpdatedFromServer = true;
                                this.HasRealData = true;
                            });
                    }
                }
            });
        }

        /// <summary>
        /// The <see cref="GarageDoorOpened" /> property's name.
        /// </summary>
        protected PremiseProperty<Boolean> _GarageDoorOpened = new PremiseProperty<bool>("GarageDoorOpened");

        /// <summary>
        /// Gets the GarageDoorOpened property.
        /// TODO Update documentation:
        /// Changes to that property's value raise the PropertyChanged event. 
        /// This property's value is broadcasted by the Messenger's default instance when it changes.
        /// </summary>
       // [PremiseProperty]
        public Boolean GarageDoorOpened
        {
            get
            {
                return _GarageDoorOpened.Value;
            }

            set
            {
                if (_GarageDoorOpened.Value == value)
                {
                    return;
                }

                var oldValue = _GarageDoorOpened.Value;
                _GarageDoorOpened.Value = value;

                // Update bindings and sendToServer change using GalaSoft.MvvmLight.Messenging
                RaisePropertyChanged(OpenCloseCommandPropertyName, "", "", false);
                RaisePropertyChanged(_GarageDoorOpened.PropertyName, oldValue, value, _GarageDoorOpened.UpdatedFromServer);
            }
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

        /// <summary>
        /// The <see cref="OpenCloseCommand" /> property's name.
        /// </summary>
        public const string OpenCloseCommandPropertyName = "OpenCloseCommand";


        /// <summary>
        /// Gets the OpenCloseCommand property.
        /// TODO Update documentation:
        /// Changes to that property's value raise the PropertyChanged event. 
        /// This property's value is broadcasted by the Messenger's default instance when it changes.
        /// </summary>
        public String OpenCloseCommand
        {
            get
            {
                return GarageDoorOpened ? "Close" : "Open" ;
            }

            set
            {
                /*
                if (_OpenCloseCommand == value)
                {
                    return;
                }

                var oldValue = _OpenCloseCommand;
                _OpenCloseCommand = value;

                // Remove one of the two calls below
                throw new NotImplementedException();

                // Update bindings, no broadcast
                RaisePropertyChanged(OpenCloseCommandPropertyName);

                // Update bindings and broadcast change using GalaSoft.MvvmLight.Messenging
                RaisePropertyChanged(OpenCloseCommandPropertyName, oldValue, value, true);
                 * */
            }
        }
     }
}
