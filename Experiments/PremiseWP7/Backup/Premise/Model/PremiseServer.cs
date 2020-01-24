using System;
using System.Windows.Threading;
using GalaSoft.MvvmLight.Messaging;
using GalaSoft.MvvmLight;
using System.Diagnostics;
using System.Reflection;
using System.Collections.Generic;
using Premise;
using System.IO.IsolatedStorage;
using System.Xml.Serialization;
using Microsoft.Phone.Shell;

/// <summary>
/// The Premise namespace contains all classes that are independent of application. 
/// The "Model" in MVVM...
/// </summary>
namespace Premise.Model
{
    /// <summary>
    /// PremiseServer is the model for the Premise server. It handles all sets and gets from the 
    /// SOAP interface exposed by Premise. It relies heavily on HttpHelper.
    /// SYS URL format.
    /// a - SubscribeToProperty
    /// 	    http://localhost/sys/Home?a?targetelementid??Brightness?subid?subname
    /// b - SubscribeToClass
    /// 	    http://localhost/sys/object?b?targetelementid?action?class?subscription_id?subscription_name
    /// 	    Class subscriptions don't get prop change events, just add/delete
    /// c - Unsubscribe
    /// d - InvokeMethod (e.g. url = http://localhost/sys/Home?d??TestFn(True, "Gold").
    ///         * Works with script functions.  Use method.TestFn, method.ParamName1, etc...
    ///         * Does not work with script methods.
    ///         * Does not appear to work with built-in object methods (e.g. IsOfExplicitType(type) fails).
    /// e - SetValue (e.g. url = http://localhost/sys/Home/Kitchen/Overheadlight?e?PowerState, message contains
    ///                  "0" or "1"), no response message-body
    /// f - GetValue (e.g. url = http://localhost/sys/Home/Kitchen/Overheadlight?f??PowerState (Note double ??)
    ///                  no message body in request
    ///                  You *can* put a targetelement between the double ??, but not really necessary here.
    ///                  response message body contains value
    /// g - RunCommand
    /// 
    /// 
    /// </summary>
    public sealed class PremiseServer
    {
        // Singleton pattern (pattern #4) from John Skeet: http://csharpindepth.com/Articles/General/Singleton.aspx
        static readonly PremiseServer instance = new PremiseServer();

        // Explicit static constructor to tell C# compiler
        // not to mark type as beforefieldinit
        static PremiseServer()
        {
        }

        public static PremiseServer Instance
        {
            get
            {
                return instance;
            }
        }

        private const Int32 TIMER_REQUERY_INTERVAL = 200;  // 200 miliseconds
        private ConnectionSettings _Settings = new ConnectionSettings();
        // Dictionary of location (e.g. "sys://xyz") & objects
        private Dictionary<String, PremiseObject> premiseObjects = new Dictionary<String, PremiseObject>();

        PremiseServer()
        {
            BuildClassMapper();

            // Registering for PropertyChangedMessageBase will give all changes. 
            Messenger.Default.Register<PremiseObjectPropertyChangeMessage>(this, (action) => PremiseObjectPropertyChangedHandler(action.PremiseObject, action.PropertyName, action.NewValue));
            Messenger.Default.Register<PremiseObjectCreatedMessage>(this, (action) => PremiseObjectCreatedHandler(action.PremiseObject));
        }

        /// <summary>
        /// Keeps a mapping of names to types so we can create the right objects
        /// PremiseObject type name, LocalTypeName (class name)
        /// </summary>
        private Dictionary<string, string> classMappings = new Dictionary<string, string>();
        void BuildClassMapper()
        {
            // Find all classes derived from PremiseObject.  Add their class name and the value of their PremiseType static field
            // to the dictionary. 
            var assembly = Assembly.GetExecutingAssembly();

            // Create one just to test.
            PremiseObject o = new PremiseObject();
            Type PremiseType = o.GetType();

            foreach (var type in assembly.GetTypes())
            {
                if (type == PremiseType || type.IsSubclassOf(PremiseType))
                {
                    FieldInfo fi = type.GetField("PremiseType");
                    // sys, local
                    classMappings.Add(fi.GetValue(PremiseType).ToString(), type.FullName);
                }
            }
        }

        /// <summary>
        /// Holds a ConnectionSettings object for persisting settings
        /// </summary>
        public ConnectionSettings Settings
        {
            get
            {
                return _Settings;
            }

            set
            {
                _Settings = value;
            }
        }

        private const string SETTINGS_FILE_NAME = "ConnectionSettings.dat";
        /// <summary>
        /// Hydrate settings from backing store.
        /// </summary>
        /// <param name="fromIsoStore">True: Use IsolatedStorage, False: Use PhoneApplicationState</param>
        public void LoadSettings(bool fromIsoStore)
        {
            ConnectionSettings connectionSettings = null;

            if (fromIsoStore)
            {
                using (IsolatedStorageFile isf = IsolatedStorageFile.GetUserStoreForApplication())
                {
                    if (isf.FileExists(SETTINGS_FILE_NAME))
                    {
                        using (IsolatedStorageFileStream fs = isf.OpenFile(SETTINGS_FILE_NAME, System.IO.FileMode.Open))
                        {
                            XmlSerializer ser = new XmlSerializer(typeof(ConnectionSettings));
                            object obj = ser.Deserialize(fs);

                            if (obj != null)
                                connectionSettings = obj as ConnectionSettings;
                        }
                    }
                }
            }
            else
            {
                if (PhoneApplicationService.Current.State.ContainsKey("UnsavedConnectionSettings"))
                {
                    connectionSettings = PhoneApplicationService.Current.State["UnsavedConnectionSettings"] as ConnectionSettings;
                    PhoneApplicationService.Current.State.Remove("UnsavedConnectionSettings");
                }
            }

            if (connectionSettings == null)
                connectionSettings = new ConnectionSettings();

            Settings = connectionSettings;
        }

        /// <summary>
        /// Save settings to backing store.
        /// </summary>
        /// <param name="fromIsoStore">True: Use IsolatedStorage, False: Use PhoneApplicationState</param>
        public void SaveSettings(bool fromIsoStore)
        {
            if (fromIsoStore)
            {
                using (IsolatedStorageFile isf = IsolatedStorageFile.GetUserStoreForApplication())
                {
                    using (IsolatedStorageFileStream fs = isf.CreateFile(SETTINGS_FILE_NAME))
                    {
                        XmlSerializer ser = new XmlSerializer(typeof(ConnectionSettings));
                        ser.Serialize(fs, Settings);
                    }
                }
            }
            else
                PhoneApplicationService.Current.State.Add("UnsavedConnectionSettings", Settings);
        }


        /// <summary>
        /// The <see cref="Host" /> property's name.
        /// </summary>
        public const string HostPropertyName = "Host";

        /// <summary>
        /// Gets the Host (servername) property.
        /// </summary>
        public String Host
        {
            get
            {
                return _Settings.Host;
            }

            set
            {
                if (_Settings.Host == value)
                {
                    return;
                }

                var oldValue = _Settings.Host;
                _Settings.Host = value;
            }
        }

        /// <summary>
        /// The <see cref="Port" /> property's name.
        /// </summary>
        public const string PortPropertyName = "Port";

        /// <summary>
        /// Gets the Port property.
        /// </summary>
        public int Port
        {
            get
            {
                return _Settings.Port;
            }

            set
            {
                if (_Settings.Port == value)
                {
                    return;
                }

                var oldValue = _Settings.Port;
                _Settings.Port = value;
            }
        }

        /// <summary>
        /// The <see cref="SSL" /> property's name.
        /// </summary>
        public const string SSLPropertyName = "SSL";

        /// <summary>
        /// Gets the SSL property.
        /// </summary>
        public bool SSL
        {
            get
            {
                return _Settings.SSL;
            }

            set
            {
                if (_Settings.SSL == value)
                {
                    return;
                }

                var oldValue = _Settings.SSL;
                _Settings.SSL = value;
            }
        }

        /// <summary>
        /// The <see cref="Username" /> property's name.
        /// </summary>
        public const string UsernamePropertyName = "Username";

        /// <summary>
        /// Gets the Username property.
        /// </summary>
        public String Username
        {
            get
            {
                return _Settings.Username;
            }

            set
            {
                if (_Settings.Username == value)
                {
                    return;
                }

                var oldValue = _Settings.Username;
                _Settings.Username = value;
            }
        }

        /// <summary>
        /// The <see cref="Password" /> property's name.
        /// </summary>
        public const string PasswordPropertyName = "Password";

        /// <summary>
        /// Gets the Password property.
        /// </summary>
        public String Password
        {
            get
            {
                return _Settings.Password;
            }

            set
            {
                if (_Settings.Password == value)
                {
                    return;
                }

                var oldValue = _Settings.Password;
                _Settings.Password = value;
            }
        }

        /// <summary>
        /// PremiseObjects are really singletons per location. This is the factory function for
        /// either creating a new one or retriving ane existing one from the premiseObjects dictionary.
        /// 
        /// TODO: Deal with the fact that premiseObjectType is not REALLY the type on the server; 
        /// we have things like QuickItems which are composites of mulitple server side objects. Nothing's
        /// broken because of this, but the terminology is mis-leading.
        /// </summary>
        /// <param name="premiseObjectType"></param>
        /// <param name="location"></param>
        /// <returns></returns>
        public PremiseObject GetOrCreateServerObject(string premiseObjectType, string location, string displayName = "", string value = "", string page = "")
        {
            PremiseObject po = null;
            if (!premiseObjects.TryGetValue(location, out po))
            {
                string classname;
                if (classMappings.TryGetValue(premiseObjectType, out classname))
                    po = (PremiseObject)Activator.CreateInstance(Type.GetType(classname), location);
                if (po != null)
                {
                    if (displayName != "") po.DisplayName = displayName;
                    if (page != "") po.Page = page;
                    if (value != "") po.Value = value;
                }
            }
            else
            {
                Debug.WriteLine("Already exists: {0}", location);
            }

            return po;
        }

        /// <summary>
        /// Whenever a PremiseObject is created it uses MvvmLight messaging to notify. This is the
        /// message handler.
        /// 
        /// TODO: This is wonky. First, there's no need to use messaging between entities within the 
        /// Model (PremiseServer/PremiseObject).  Second, there's probably a cleaner pattern to use for
        /// Singleton instances like this.
        /// </summary>
        /// <param name="so"></param>
        void PremiseObjectCreatedHandler(PremiseObject so)
        {
            //Debug.Assert(!premiseObjects.ContainsKey(so.Location));
            if (so != null && so.Location != "")
                premiseObjects.Add(so.Location, so);
            else
                Debug.WriteLine("This is not good! PremiseServer.PremiseCreatedHandler so = null");
        }

        // QueryAllObjects requeries the server immediately
        public void QueryAllObjects()
        {
            foreach (PremiseObject so in premiseObjects.Values)
                so.QueryServer(this);
        }

        // Callers can either use this or can direclty call PremiseObject.QueryServer
        public void QueryObects(List<PremiseObject> ObjectList)
        {
//            System.ComponentModel.BackgroundWorker bw = new System.ComponentModel.BackgroundWorker();

//            bw.DoWork += (sender, e) =>
            //{
                foreach (PremiseObject po in ObjectList)
                    po.QueryServer(this);
            //};

//            bw.RunWorkerAsync();

        }

        // RequeryAllObjectsWithDelay sets a requeries the server after a TIMER_REQUERY_INTERVAL ms delay.
        // We have to wait just a little bit, or we get back the wrong value as the driver hasn't
        // processed the command yet.  Example, if we set PowerState to true, the brightness doesn't get
        // set properly until the driver processes the powerstate command.
        public void RequeryAllObjectsWithDelay()
        {
            System.Windows.Threading.DispatcherTimer timerRequery;
            timerRequery = new System.Windows.Threading.DispatcherTimer();
            timerRequery.Interval = new TimeSpan(0, 0, 0, 0, TIMER_REQUERY_INTERVAL);
            timerRequery.Tick += (t, args) =>
                {
                    timerRequery.Stop();
                    QueryAllObjects();
                };
            timerRequery.Start();
        }

        // Whenver a property on any PremiseObject in the app changes this gets called.
        // Sends the changed property to the server.
        void PremiseObjectPropertyChangedHandler(PremiseObject so, string PropertyName, object Value)
        {
            //Debug.WriteLine("PropertyChangeHandler(" + so.Location + ", " + PropertyName + ",  " + so.DisplayName + ") Property: " + Value.ToString());

            // Don't allow changing base properties (ID, Location, Name) (for safety's sake)
            if (PropertyName == "HasRealData" ||
                PropertyName == "Location" ||
                PropertyName == "ID" ||
                PropertyName == "Name")
                return;

            SetPropertyAsync(so.Location, PropertyName, Value.ToString());
        }

        /// <summary>
        /// Converts a sys URI (sys://Home/...) to an HTTP URL (http://server/sys/Home/...)
        /// </summary>
        public string GetUrlFromSysUri(string SysUrl)
        {
            // Remove the "sys:/" (5 characters) and replace with the base server Uri
            Uri uriServer;
            if (SSL)
                uriServer = new UriBuilder("https", Host, Port, "sys").Uri;
            else
                uriServer = new UriBuilder("http", Host, Port, "sys").Uri;

            return uriServer.AbsoluteUri + SysUrl.Remove(0, 5);
        }

        /// <summary>
        ///     e - SetValue (e.g. url = http://localhost/sys/Home/Kitchen/Overheadlight?e?PowerState, message contains
        ///                  "0" or "1"), no response message-body
        /// </summary>
        private void SetPropertyAsync(string location, string property, string value)
        {
            Uri uri = new Uri(GetUrlFromSysUri(location) + "?e?" + property);
            Debug.WriteLine("SetPropertyAsync: url = <" + uri.ToString() + "> value=" + value.ToString());
            HttpHelper helper = new HttpHelper(uri, "POST", value, false, null, Username, Password);
            helper.Execute();
        }

        public delegate void GetPropertyCompletionMethod(HttpResponseCompleteEventArgs e);
        /// <summary>
        /// f - GetValue (e.g. url = http://localhost/sys/Home/Kitchen/Overheadlight?f??PowerState (Note double ??)
        ///                  no message body in request
        ///                  You *can* put a targetelement between the double ??, but not really necessary here.
        ///                  response message body contains value
        /// </summary>
        public void GetPropertyAsync<T>(string location, T property, GetPropertyCompletionMethod cm)
        {
            Uri uri = new Uri(GetUrlFromSysUri(location) + "?f??" + property);
            HttpHelper helper = new HttpHelper(uri, "POST", null, true, property, Username, Password);
            //Debug.WriteLine("GetPropertyAsync: url = <" + uri.ToString() + ">");
            helper.ResponseComplete += new HttpResponseCompleteEventHandler(cm);
            helper.Execute();
        }

        public delegate void InvokeMethodCompletionMethod(HttpResponseCompleteEventArgs e);
        /// <summary>
        /// d - InvokeMethod
        /// </summary>
        public void InvokeMethodAsync<T>(string location, T method, GetPropertyCompletionMethod cm, params object[] parameters)
        {
            Uri uri = new Uri(GetUrlFromSysUri(location) + "?d??" + method);
            HttpHelper helper = new HttpHelper(uri, "POST", null /*parameters.ToString()*/, true, method, Username, Password);
            //Debug.WriteLine("GetPropertyAsync: url = <" + uri.ToString() + ">");
            helper.ResponseComplete += new HttpResponseCompleteEventHandler(cm);
            helper.Execute();
        }



        /// <summary>
        /// The ConnectComplete event is fired when a Connect try has completed. e.Succeeded will
        /// be true if the DisplayName of the Home object has been retrieved. If false e.Response
        /// will contain the error string.
        /// </summary>
        public event ConnectCompleteEventHandler ConnectComplete;

        // ConnectComplete event handler arguments. These mirror the HttpResponseCompleteEventArgs.
        public delegate void ConnectCompleteEventHandler(object sender, ConnectCompleteEventArgs e);
        public class ConnectCompleteEventArgs : EventArgs
        {
            public string Response;
            public bool Succeeded;
            public ConnectCompleteEventArgs(string response, bool Succeeded)
            {
                this.Response = response;
                this.Succeeded = Succeeded;
            }
        }

        /// <summary>
        /// Connect simply attempts to retrieve the DisplayName property fo the Home object. It fires
        ///  the ConnectComplete event when completed.  eSucceeded is True if DisplayName was recieved
        /// </summary>
        public void Connect()
        {
            Debug.WriteLine("PremiseServer.Connect: {0}", GetUrlFromSysUri("sys://Home"));
            GetPropertyAsync("sys://Home", "DisplayName", (e) => 
                {
                    if (ConnectComplete != null)
                        ConnectComplete(this, new ConnectCompleteEventArgs(e.Response, e.Succeeded));
                });

        }

    }

}