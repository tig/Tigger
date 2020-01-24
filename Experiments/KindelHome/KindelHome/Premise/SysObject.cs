using System.Collections.Generic;
using System;
using System.Windows.Threading;
using System.Windows.Controls;
using System.Windows.Media.Imaging;
using KindelGDO;
using Premise;
using System.Diagnostics;
using GalaSoft.MvvmLight;
using GalaSoft.MvvmLight.Messaging;
using System.Reflection;

namespace Premise
{
    //
    // Summary:
    //      Base object for Premise SYS objects. Derives from the MvvmLight ViewModelBase to support
    //      broadcasting property changes on MvvmLight.Messaging.
    //      
    //      Also overrides RaisePropertyChanged to test if we are calling from the UI thread
    //      or not.  If not, use CheckBeginInvokeOnUI to queue on the UI thread.
    //
    //  Properties: 
    //
    //  ID          - Sys object ID (GUID)
    //  Location    - Sys object location (e.g. sys://Home/Upstairs/Kitchen/Motion Detector)
    //  Name        
    //  DisplayName
    //  Icon
    //
    public class SysObject : ViewModelBase
    {
        public static string SysType = "Object";
        public SysObject()
        {
            // When we are initilizing, do not send updates to the server
            UpdateServerWithChange = false;

            if (IsInDesignMode)
            {
                // Code runs in Blend --> create design time data.
            }
            else
            {
                // Code runs "for real": Connect to service, etc...

                Messenger.Default.Send<SysObjectCreatedMessage>(new SysObjectCreatedMessage(this));
            }

        }

        /// <summary>
        /// UpdateServerWithChange tells the object whether property change
        /// notifications should be sent to the PremiseSever or not.
        /// </summary>
        public bool UpdateServerWithChange { get; set; }

        /// <summary>
        /// The <see cref="HasRealData" /> property's name.
        /// </summary>
        public const string HasRealDataPropertyName = "HasRealData";
        private bool _HasRealData = false;

        /// <summary>
        /// Gets the HasRealData property.
        /// HasReadlData indicates whether the object has been initizlied with real data from the server
        /// Changes to that property's value raise the PropertyChanged event. 
        /// This property's value is broadcasted by the Messenger's default instance when it changes.
        /// </summary>
        public bool HasRealData
        {
            get
            {
                return _HasRealData;
            }

            set
            {
                if (_HasRealData == value)
                {
                    return;
                }

                var oldValue = _HasRealData;
                _HasRealData = value;

                // Update bindings and broadcast change using GalaSoft.MvvmLight.Messenging
                RaisePropertyChanged(HasRealDataPropertyName, oldValue, value, false);
            }
        }

        /// The <see cref="ID" /> property's name.
        /// </summary>
        public const string IDPropertyName = "ID";

        protected String _ID = "";

        /// <summary>
        /// Gets the ID property.
        /// TODO Update documentation:
        /// Changes to that property's value raise the PropertyChanged event. 
        /// This property's value is broadcasted by the Messenger's default instance when it changes.
        /// </summary>
        //[SysProperty]
        public String ID
        {
            get
            {
                return _ID;
            }

            set
            {
                if (_ID == value)
                {
                    return;
                }

                var oldValue = _ID;
                _ID = value;

                // Update bindings and broadcast change using GalaSoft.MvvmLight.Messenging
                RaisePropertyChanged(IDPropertyName, oldValue, value, false);
            }
        }
         
        /// <summary>
        /// The <see cref="Location" /> property's name.
        /// </summary>
        protected SysProperty<String> _Location = new SysProperty<string>("Location");

        /// <summary>
        /// Gets the Location property.
        /// TODO Update documentation:
        /// Changes to that property's value raise the PropertyChanged event. 
        /// This property's value is broadcasted by the Messenger's default instance when it changes.
        /// </summary>
        //[SysProperty]
        public String Location
        {
            get
            {
                return _Location.Value;
            }

            set
            {
                _Location.Object = this; // Always make sure the SysProperty knows it's containing object
                if (_Location.Value == value)
                {
                    return;
                }

                var oldValue = _Location.Value;
                _Location.Value = value;

                // Update bindings and broadcast change using GalaSoft.MvvmLight.Messenging
                RaisePropertyChanged(_Location.PropertyName, oldValue, value, _Location.UpdatedFromServer);
            }
        }


        /// <summary>
        /// The <see cref="Name" /> property's name.
        /// </summary>
        protected SysProperty<String> _Name = new SysProperty<string>("Name");

        /// <summary>
        /// Gets the Name property.
        /// TODO Update documentation:
        /// Changes to that property's value raise the PropertyChanged event. 
        /// This property's value is broadcasted by the Messenger's default instance when it changes.
        /// </summary>
//        [SysProperty]
        public string Name
        {
            get
            {
                return _Name.Value;
            }

            set
            {
                _Name.Object = this; // Always make sure the SysProperty knows it's containing object
                if (_Name.Value == value)
                {
                    return;
                }

                var oldValue = _Name.Value;
                _Name.Value = value;

                // Update bindings and broadcast change using GalaSoft.MvvmLight.Messenging
                RaisePropertyChanged(_Name.PropertyName, oldValue, value, _Name.UpdatedFromServer);
            }
        }

        /// <summary>
        /// The <see cref="DisplayName" /> property's name.
        /// </summary>
        //public const string DisplayNamePropertyName = "DisplayName";

        protected SysProperty<String> _DisplayName = new SysProperty<String>("DisplayName");

        /// <summary>
        /// Gets the DisplayName property.
        /// TODO Update documentation:
        /// Changes to that property's value raise the PropertyChanged event. 
        /// This property's value is broadcasted by the Messenger's default instance when it changes.
        /// </summary>
        //[SysProperty]
        public string DisplayName
        {
            get
            {
                return _DisplayName.Value;
            }

            set
            {
                _DisplayName.Object = this; // Always make sure the SysProperty knows it's containing object
                if (_DisplayName.Value == value)
                {
                    return;
                }

                var oldValue = _DisplayName.Value;
                _DisplayName.Value = value;

                // Update bindings and broadcast change using GalaSoft.MvvmLight.Messenging
                RaisePropertyChanged(_DisplayName.PropertyName, oldValue, value, _DisplayName.UpdatedFromServer);
            }
        }

        /// <summary>
        /// The <see cref="Icon" /> property's name.
        /// </summary>
        //public const string IconPropertyName = "Icon";

        protected SysProperty<Image> _Icon = new SysProperty<Image>("Icon");

        /// <summary>
        /// Gets the Icon property.
        /// TODO Update documentation:
        /// Changes to that property's value raise the PropertyChanged event. 
        /// This property's value is broadcasted by the Messenger's default instance when it changes.
        /// </summary>
        //[SysProperty]
        public Image Icon
        {

            get
            {
                if (_Icon.Value == null)
                {
                    _Icon.Value = new Image();
                    _Icon.Value.Source = new BitmapImage(new Uri("ApplicationIcon.png", UriKind.RelativeOrAbsolute));
                }
                return _Icon.Value;
            }

            set
            {
                _Icon.Object = this; // Always make sure the SysProperty knows it's containing object
                if (_Icon.Value == value)
                {
                    return;
                }

                var oldValue = _Icon.Value;
                _Icon.Value = value;

                // Update bindings and broadcast change using GalaSoft.MvvmLight.Messenging
                RaisePropertyChanged(_Location.PropertyName, oldValue, value, _Location.UpdatedFromServer);
            }
        }

        protected override void RaisePropertyChanged<T>(string propertyName, T oldValue, T newValue, bool broadcast)
        {
            // Check if we are on the UI thread or not
            if (App.Current.RootVisual == null || App.Current.RootVisual.CheckAccess())
            {
                base.RaisePropertyChanged(propertyName, oldValue, newValue, broadcast);
            }
            else
            {
                // Invoke on the UI thread
                // Update bindings and broadcast change using GalaSoft.MvvmLight.Messenging
                GalaSoft.MvvmLight.Threading.DispatcherHelper.CheckBeginInvokeOnUI(() =>
                    base.RaisePropertyChanged(propertyName, oldValue, newValue, true));
            }

            if (broadcast)
                SendPropertyChangeToServer(propertyName, newValue.ToString());

        }

        private void SendPropertyChangeToServer(String PropertyName, String Value)
        {
            Debug.WriteLine("SendPropertyChangeToServer(\"{0}\", \"{1}\")", PropertyName, Value);
            Messenger.Default.Send<SysObjectPropertyChangeMessage>(new SysObjectPropertyChangeMessage(this, PropertyName, Value));
        }

        // Called from PremiseServer when a result has been returned from the server.
        // Uses reflection to set the appropriate property's value 
        public void PropertyGetCompleteHandler(HttpResponseCompleteEventArgs e)
        {
            Debug.WriteLine("{0}::PropertyGetCompleteHandler({1}): \"{2}\"", this._Location, (string)e.context, e.Response.ToString());

//            Action<Object> action;
//            if (PropertyTable.TryGetValue((string)e.context, out action))
//                action(e.Response); 
//            else
//                Debug.WriteLine("The action \"{0}\" was not found", e.context);

            // Notify clients that this object now has real data (enable UI)
            GalaSoft.MvvmLight.Threading.DispatcherHelper.CheckBeginInvokeOnUI(() => this.HasRealData = true);

        }

        public virtual void QueryServer(PremiseServer server)
        {
            // Enumerate through each property 
            // Name, DisplayName, Icon
            server.GetPropertyAsync(Location, "Name", (args) => 
            {
                if (args.Succeeded)
                {
                    GalaSoft.MvvmLight.Threading.DispatcherHelper.CheckBeginInvokeOnUI(() =>
                        {
                            this.Name = (String)args.Response;
                            this._Name.UpdatedFromServer = true;
                            this.HasRealData = true;
                        });
                }
            });
            
            server.GetPropertyAsync(Location, "DisplayName", (args) =>
            {
                if (args.Succeeded)
                {
                    GalaSoft.MvvmLight.Threading.DispatcherHelper.CheckBeginInvokeOnUI(() =>
                        {
                            this.DisplayName = (String)args.Response;
                            this._DisplayName.UpdatedFromServer = true;
                            this.HasRealData = true;
                        });
                }
            });
            /*
            server.GetPropertyAsync(Location, "Icon", (args) =>
            {
                if (args.Succeeded)
                {
                    GalaSoft.MvvmLight.Threading.DispatcherHelper.CheckBeginInvokeOnUI(() => 
                        {
                            this.Icon = (String)args.Response;
                            this._Icon.UpdatedFromServer = true;
                            this.HasRealData = true;
                        });
                }
            });
             * */

        }

    }

    class SysObjectCreatedMessage : MessageBase
    {
        public SysObject SysObject = null;
        public SysObjectCreatedMessage(SysObject o)
        {
            this.SysObject = o;
        }
    }

    class SysObjectPropertyChangeMessage : MessageBase
    {
        public SysObject SysObject = null;
        public String PropertyName = "";
        public object NewValue;

        public SysObjectPropertyChangeMessage(SysObject o, string PropertyName, object NewValue)
        {
            this.SysObject = o;
            this.PropertyName = PropertyName;
            this.NewValue = NewValue;
        }

    }
}


