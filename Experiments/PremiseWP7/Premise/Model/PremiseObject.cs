using System.Collections.Generic;
using System;
using System.Windows.Threading;
using System.Windows.Data;
using System.Windows.Controls;
using System.Windows.Media.Imaging;
using System.Diagnostics;
using System.Reflection;
using GalaSoft.MvvmLight;
using GalaSoft.MvvmLight.Messaging;
using GalaSoft.MvvmLight.Threading;
using Premise;

namespace Premise.Model
{

    public sealed class PremiseObjects : Dictionary<string, PremiseObject>
    {
        // Singleton pattern (pattern #4) from John Skeet: http://csharpindepth.com/Articles/General/Singleton.aspx
        static readonly PremiseObjects instance = new PremiseObjects();

        // Explicit static constructor to tell C# compiler
        // not to mark type as beforefieldinit
        static PremiseObjects()
        {
        }

        PremiseObjects()
        {
        }

        public bool TryGetObject(string type, string location, out PremiseObject outObject)
        {
            bool f = false;

            outObject = null;

            return f;
        }

        public static PremiseObjects Instance
        {
            get
            {
                return instance;
            }
        }
    }


    //
    // Summary:
    //      Base object for Premise SYS objects. Derives from the MvvmLight ViewModelBase to support
    //      broadcasting property changes on MvvmLight.Messaging.
    //      
    //      Also overrides RaisePropertyChanged to test if we are calling from the UI thread
    //      or not.  If not, use CheckBeginInvokeOnUI to queue on the UI thread.
    //
    //  Member properties:
    //
    //  HasRealData - True if Value was set via a result returned from the server
    //  Page        - Path to view.xaml for this object
    //
    //  Premise Properties: 
    //
    //  ID          - Sys object ID (GUID)
    //  Location    - Sys object location (e.g. sys://Home/Upstairs/Kitchen/Motion Detector)
    //  Name        
    //  DisplayName
    //  Icon
    //
    public class PremiseObject : ViewModelBase
    {
        public static string PremiseType = "PremiseObject";
        public PremiseObject(String location)
        {
            Location = location;
            if (IsInDesignMode)
            {
                // Code runs in Blend --> create design time data.
            }
            else
            {
                // Code runs "for real": Connect to service, etc...

                // Whenever a PremiseObject is created PremiseServer keeps track of it.
                Messenger.Default.Send<PremiseObjectCreatedMessage>(new PremiseObjectCreatedMessage(this));
            }
        }

        public PremiseObject() 
        {
            // for design time and for creating a fake object for PremiseServer.BuildClassMapper
        }

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

                // Update bindings and sendToServer change using GalaSoft.MvvmLight.Messenging
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
        //[PremiseProperty]
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

                // Update bindings and sendToServer change using GalaSoft.MvvmLight.Messenging
                RaisePropertyChanged(IDPropertyName, oldValue, value, false);
            }
        }
         
        /// <summary>
        /// The <see cref="Location" /> property's name.
        /// </summary>
        protected PremiseProperty<String> _Location = new PremiseProperty<string>("Location", true, true);

        /// <summary>
        /// Gets the Location property.
        /// TODO Update documentation:
        /// Changes to that property's value raise the PropertyChanged event. 
        /// This property's value is broadcasted by the Messenger's default instance when it changes.
        /// </summary>
        public String Location
        {
            get
            {
                return _Location.Value;
            }

            set
            {
                if (_Location.Value == value)
                {
                    return;
                }

                var oldValue = _Location.Value;
                _Location.Value = value;

                // Update bindings 
                RaisePropertyChanged(_Location.PropertyName, oldValue, value, false);
            }
        }


        /// <summary>
        /// The <see cref="Value" /> property's name.
        /// </summary>
        protected PremiseProperty<String> _Value = new PremiseProperty<string>("Value");

        /// <summary>
        /// Gets the Value property.
        /// 
        /// Value is a special property. On premise it is generated and is not actually part of an 
        /// object's model. It is not available via the SOAP interface.  So we duplicate the idea on the
        /// client side. 
        /// 
        /// Derived PremiseObjects can set ValuePropertyName to the name of whatever property they 
        /// want as their value property. Eg. a Light wants PowerState to be it's value property. 
        /// It would set ValuePropertyName to "PowerState".
        /// 
        /// In addition derived classes can override FormatValue to dynamically change what clients
        /// see.
        /// 
        /// Changes to that property's value raise the PropertyChanged event. 
        /// This property's value is broadcasted by the Messenger's default instance when it changes.
        /// </summary>
        public string Value
        {
            get
            {
                return FormatValue(_Value.Value);
            }

            set
            {
                //if (_Value.Value == value)
                //{
                //    return;
                //}

                var oldValue = _Value.Value;
                _Value.Value = value;

                // Derived classes can override ValueSet
                ValueSet();

                // Update bindings and sendToServer change using GalaSoft.MvvmLight.Messenging

                // Value is special. We always pass Value here so MvvmLight finds the property,
                // but the actual premise property name may have been changed by a derived sysobject
                // class.
                //
                // Also we always set sendToServer to false because we never want to update the server
                // with Value because it's not a valid property name for Premise
                RaisePropertyChanged("Value", oldValue, value, false);

            }
        }


        // Value_Get is virtual because Value is never a real premise property in the SOAP interface
        // Derived PremiseObject classes may wish to override this.
        protected virtual void Value_Get(PremiseServer server)
        {
            if (_Value.CheckGetFromServer == false) return;

            String propertyName = _Value.PropertyName;

            if (propertyName == "Value") return;

            server.GetPropertyAsync(Location, propertyName, (HttpResponseArgs) =>
            {
                if (HttpResponseArgs.Succeeded)
                {
                    //Debug.WriteLine("Received {0}: {1} = {2}", DisplayName, propertyName, HttpResponseArgs.Response);
                    DispatcherHelper.CheckBeginInvokeOnUI(() =>
                    {
                        Value = (String)HttpResponseArgs.Response; // <-- this is the whole cause of this confusing architecture
                        _Value.UpdatedFromServer = true;
                        HasRealData = true;
                    });
                }
            });
        }

        protected virtual void ValueSet()
        {
        }

        public virtual String FormatValue(object value)
        {
            if (value != null)
                return value.ToString();
            return "no status";
        }

        /// <summary>
        /// The <see cref="Name" /> property's name.
        /// </summary>
        protected PremiseProperty<String> _Name = new PremiseProperty<string>("Name", true, true);

        /// <summary>
        /// Gets the Name property.
        /// TODO Update documentation:
        /// Changes to that property's value raise the PropertyChanged event. 
        /// This property's value is broadcasted by the Messenger's default instance when it changes.
        /// </summary>
        public string Name
        {
            get
            {
                return _Name.Value;
            }

            set
            {
                if (_Name.Value == value)
                {
                    return;
                }

                var oldValue = _Name.Value;
                _Name.Value = value;

                // Update bindings and sendToServer change using GalaSoft.MvvmLight.Messenging
                RaisePropertyChanged(_Name.PropertyName, oldValue, value, _Name.UpdatedFromServer);
            }
        }

        protected void Name_Get(PremiseServer server)
        {
            if (_Name.CheckGetFromServer == false) return;

            String propertyName = _Name.PropertyName;

            server.GetPropertyAsync(Location, propertyName, (HttpResponseArgs) =>
            {
                if (HttpResponseArgs.Succeeded)
                {
                    //Debug.WriteLine("Received {0}: {1} = {2}", DisplayName, propertyName, HttpResponseArgs.Response);
                    DispatcherHelper.CheckBeginInvokeOnUI(() =>
                    {
                        Name = (String)HttpResponseArgs.Response; // <-- this is the whole cause of this confusing architecture
                        _Name.UpdatedFromServer = true;
                        HasRealData = true;
                    });
                }
            });
        }


        /// <summary>
        /// The <see cref="DisplayName" /> property's name.
        /// </summary>
        //public const string DisplayNamePropertyName = "DisplayName";

        protected PremiseProperty<String> _DisplayName = new PremiseProperty<String>("DisplayName");

        public void SetDisplayNameIgnoreFromServer()
        {
            _DisplayName.IgnoreServer = true;
        }

        /// <summary>
        /// Gets the DisplayName property.
        /// TODO Update documentation:
        /// Changes to that property's value raise the PropertyChanged event. 
        /// This property's value is broadcasted by the Messenger's default instance when it changes.
        /// </summary>
        //[PremiseProperty]
        public string DisplayName
        {
            get
            {
                return _DisplayName.Value;
            }

            set
            {
                if (_DisplayName.Value == value)
                {
                    return;
                }

                var oldValue = _DisplayName;
                _DisplayName.Value = value;

                // Update bindings and sendToServer change using GalaSoft.MvvmLight.Messenging
                RaisePropertyChanged(_DisplayName.PropertyName, oldValue, _DisplayName, _DisplayName.UpdatedFromServer);
            }
        }

        protected void DisplayName_Get(PremiseServer server)
        {
            if (!_DisplayName.CheckGetFromServer) return;

            String propertyName = _DisplayName.PropertyName;

            _DisplayName.UpdatedFromServer = false;
            server.GetPropertyAsync(Location, propertyName, (HttpResponseArgs) =>
            {
                if (HttpResponseArgs.Succeeded)
                {
                    //Debug.WriteLine("Received {0}: {1} = {2}", DisplayName, propertyName, HttpResponseArgs.Response);
                    DispatcherHelper.CheckBeginInvokeOnUI(() =>
                    {
                        DisplayName = (String)HttpResponseArgs.Response; // <-- this is the whole cause of this confusing architecture
                        _DisplayName.UpdatedFromServer = true;
                        HasRealData = true;
                    });
                }
            });
        }


        protected PremiseProperty<String> _Description = new PremiseProperty<String>("Description", true, false);

        /// <summary>
        /// Gets the Description property.
        /// TODO Update documentation:
        /// Changes to that property's value raise the PropertyChanged event. 
        /// This property's value is broadcasted by the Messenger's default instance when it changes.
        /// </summary>
        //[PremiseProperty]
        public string Description
        {
            get
            {
                return _Description.Value;
            }

            set
            {
                if (_Description.Value == value)
                {
                    return;
                }

                var oldValue = _Description;
                _Description.Value = value;

                // Update bindings and sendToServer change using GalaSoft.MvvmLight.Messenging
                RaisePropertyChanged(_Description.PropertyName, oldValue, _Description, _Description.UpdatedFromServer);
            }
        }


        protected void Description_Get(PremiseServer server)
        {
            if (!_Description.CheckGetFromServer) return;

            String propertyName = _Description.PropertyName;

            _Description.UpdatedFromServer = false;
            server.GetPropertyAsync(Location, propertyName, (HttpResponseArgs) =>
            {
                if (HttpResponseArgs.Succeeded)
                {
                    //Debug.WriteLine("Received {0}: {1} = {2}", DisplayName, propertyName, HttpResponseArgs.Response);
                    DispatcherHelper.CheckBeginInvokeOnUI(() =>
                    {
                        Description = (String)HttpResponseArgs.Response; // <-- this is the whole cause of this confusing architecture
                        _Description.UpdatedFromServer = true;
                        HasRealData = true;
                    });
                }
            });
        }


        /// <summary>
        /// The <see cref="Icon" /> property's name.
        /// </summary>
        //public const string IconPropertyName = "Icon";

        protected PremiseProperty<Image> _Icon = new PremiseProperty<Image>("Icon", true);

        /// <summary>
        /// Gets the Icon property.
        /// TODO Update documentation:
        /// Changes to that property's value raise the PropertyChanged event. 
        /// This property's value is broadcasted by the Messenger's default instance when it changes.
        /// </summary>
        //[PremiseProperty]
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
                if (_Icon.Value == value)
                {
                    return;
                }

                var oldValue = _Icon.Value;
                _Icon.Value = value;

                // Update bindings and sendToServer change using GalaSoft.MvvmLight.Messenging
                RaisePropertyChanged(_Location.PropertyName, oldValue, value, _Location.UpdatedFromServer);
            }
        }

        protected void Icon_Get(PremiseServer server)
        {
            if (!_Icon.CheckGetFromServer) return;

        }

        /// <summary>
        /// Overrides ViewModelBase.
        /// 
        /// IMPORTANT - We override the meaning of the broadcast (4th) parameter. It is used to indicate whether the property
        /// change should also be sent back to the server. Without this we'd be updating the server with property changes
        /// that either occured before it was connected or in cycles.
        /// 
        /// </summary>
        protected override void RaisePropertyChanged<T>(string propertyName, T oldValue, T newValue, bool sendToServer)
        {
            if (sendToServer)
                SendPropertyChangeToServer(propertyName, newValue);

            // Check if we are on the UI thread or not
            if (App.Current.RootVisual == null || App.Current.RootVisual.CheckAccess())
            {
                // broadcast == false because I don't know why it would ever be true
                base.RaisePropertyChanged(propertyName, oldValue, newValue, false);
            }
            else
            {
                // Invoke on the UI thread
                // Update bindings 
                // broadcast == false because I don't know why it would ever be true
                GalaSoft.MvvmLight.Threading.DispatcherHelper.CheckBeginInvokeOnUI(() =>
                    base.RaisePropertyChanged(propertyName, oldValue, newValue, false));
            }

        }

        private void SendPropertyChangeToServer<T>(String PropertyName, T Value)
        {
            Debug.WriteLine("SendPropertyChangeToServer(\"{0}\", \"{1}\")", PropertyName, Value);
            Messenger.Default.Send<PremiseObjectPropertyChangeMessage>(new PremiseObjectPropertyChangeMessage(this, PropertyName, Value));
        }


        // TODO: A useful optimization on PremiseObject.QueryServer would be to 
        // avoid issuing the query if there's already a request outstanding
        public virtual void QueryServer(PremiseServer server)
        {
            // Enumerate through each property issuing an asynchronous get

            Value_Get(server);
            Name_Get(server);
            DisplayName_Get(server);
            Description_Get(server);
            Icon_Get(server);
        }

        /// <summary>
        /// E.g. "/GarageDoorsPage.xaml"
        /// </summary>
        public String Page
        {
            get;
            set;
        }

    }

    /// <summary>
    /// Message sent when a new PremiseObject is created. Enables PremiseServer to
    /// keep track of all objects created.
    /// </summary>
    class PremiseObjectCreatedMessage : MessageBase
    {
        public PremiseObject PremiseObject = null;
        public PremiseObjectCreatedMessage(PremiseObject o)
        {
            this.PremiseObject = o;
        }
    }

    /// <summary>
    /// Message sent whenever a property on a PremiseObject changes. Used to notify
    /// PremiseServer that it needs to send a change to the server.
    /// </summary>
    class PremiseObjectPropertyChangeMessage : MessageBase
    {
        public PremiseObject PremiseObject = null;
        public String PropertyName = "";
        public object NewValue;

        public PremiseObjectPropertyChangeMessage(PremiseObject o, string PropertyName, object NewValue)
        {
            this.PremiseObject = o;
            this.PropertyName = PropertyName;
            this.NewValue = NewValue;
        }

    }
}


