using System;
using System.Windows.Threading;
using GalaSoft.MvvmLight.Messaging;
using System.Diagnostics;
using KindelGDO.SysObjects;
using KindelGDO;
using System.Reflection;
using System.Collections.Generic;

/// <summary>
/// The Premise namespace contains all classes that are independent of application. 
/// The "Model" in MVVM...
/// </summary>
namespace Premise
{
    /// <summary>
    /// PremiseServer is the model for the Premise server. It handles all sets and gets from the 
    /// SOAP interface exposed by Premise. It relies heavily on HttpHelper.
    /// </summary>
    public class PremiseServer
    {
        private Uri uriServer;
        private const Int32 TIMER_REQUERY_INTERVAL = 200;  // 200 miliseconds

        /// <summary>
        /// Constructs a PremiseServer object representing the Premise server SOAP interface.
        /// </summary>
        /// <param name="servername">Servername (e.g. "premiseserver")</param>]
        /// <param name="port">Port (typicaly 80)</param>
        public PremiseServer(string servername, int port)
        {
            uriServer = new UriBuilder("http", servername, port, "sys").Uri;
            ServerName = servername;
            Port = port;

            BuildClassMapper();

            // Registering for PropertyChangedMessageBase will give all changes. 
            Messenger.Default.Register<SysObjectPropertyChangeMessage>(this, (action) => SysObjectPropertyChangedHandler(action.SysObject, action.PropertyName, action.NewValue));
            Messenger.Default.Register<SysObjectCreatedMessage>(this, (action) => SysObjectCreatedHandler(action.SysObject));
        }

        public String ServerName 
        {
            get { return uriServer.Host; }
            set
            {
                uriServer = new UriBuilder("http", value, Port, "sys").Uri;
            }
        }

        public int Port
        {
            get { return uriServer.Port; }
            set
            {
                uriServer = new UriBuilder("http", ServerName, value, "sys").Uri;
            }
        }


        private Dictionary<string, string> classMappings = new Dictionary<string,string>();

        // SysTypeName, LocalTypeName
        void BuildClassMapper()
        {
            // Find all classes derived from SysObject.  Add their class name and the value of their SysType static field
            // to the dictionary. 
            var assembly = Assembly.GetExecutingAssembly(); 
            SysObject o = new SysObject();
            Type sysType = o.GetType();

            foreach ( var type in assembly.GetTypes() )
            {
                if (type == sysType || type.IsSubclassOf(sysType))
                {
                    FieldInfo fi = type.GetField("SysType");

                    // sys, local
                    classMappings.Add(fi.GetValue(sysType).ToString(), type.FullName);
                }
            }
        }

        public SysObject NewServerObject(string systype)
        {
            string localtype;

            if (classMappings.TryGetValue(systype, out localtype))
            {
                return (SysObject)Activator.CreateInstance(Type.GetType(localtype));
            }
            else
                return null;

        }

        private List<SysObject> sysObjects = new List<SysObject>();
        void SysObjectCreatedHandler(SysObject so)
        {
            sysObjects.Add(so);
        }

        // RequeryValues requeries the server immediately
        public void RequeryValues()
        {
            OnRequery();
        }

        // RequeryValuesWithDelay sets a requeries the server after a TIMER_REQUERY_INTERVAL ms delay.
        // We have to wait just a little bit, or we get back the wrong value as the driver hasn't
        // processed the command yet.  Example, if we set PowerState to true, the brightness doesn't get
        // set properly until the driver processes the powerstate command.
        public void RequeryValuesWithDelay()
        {
            System.Windows.Threading.DispatcherTimer timerRequery;
            timerRequery = new System.Windows.Threading.DispatcherTimer();
            timerRequery.Interval = new TimeSpan(0, 0, 0, 0, TIMER_REQUERY_INTERVAL);
            Debug.WriteLine("Starting Requery Timer...");
            timerRequery.Tick += (t, args) =>
                {
                    Debug.WriteLine("Requery Timer Tick");
                    timerRequery.Stop();
                    OnRequery();
                };
            timerRequery.Start();
        }

        protected virtual void OnRequery()
        {
            Debug.WriteLine("OnRequery");
            //Type type;

            foreach (SysObject so in sysObjects)
            {
                so.QueryServer(this);
            }

        }

        // Whenver a property on any SysObject in the app changes this gets called.
        // Sends the changed property to the server.
        void SysObjectPropertyChangedHandler(SysObject so, string PropertyName, object Value)
        {
            //Debug.WriteLine("PropertyChangeHandler(" + so.Location + ", " + PropertyName + ",  " + so.DisplayName + ") Property: " + Value.ToString());

            // Don't allow changing base properties (ID, Location, Name) (for safety's sake)
            if (PropertyName == "HasRealData" ||
                PropertyName == "Location" ||
                PropertyName == "ID" ||
                PropertyName == "Name")
                return;

            // Convert all underscores to spaces in propertynames
            SetPropertyAsync(so.Location, PropertyName, Value.ToString());
        }

        /// <summary>
        /// Converts a sys URI (sys://Home/...) to an HTTP URL (http://server/sys/Home/...)
        /// </summary>
        public string GetUrlFromSysUri(string SysUrl)
        {
            // Remove the "sys:/" (5 characters) and replace with the base server Uri
            return uriServer.AbsoluteUri + SysUrl.Remove(0, 5);
        }

        private void SetPropertyAsync(string location, string property, string value)
        {
            Uri uri = new Uri(GetUrlFromSysUri(location) + "?e?" + property);
            Debug.WriteLine("SetPropertyAsync: url = <" + uri.ToString() + "> value=" + value.ToString());
            HttpHelper helper = new HttpHelper(uri, "POST", value, false /*ResponseExpected*/);
            helper.Execute();
        }

        public delegate void GetPropertyCompletionMethod(HttpResponseCompleteEventArgs e);
        public void GetPropertyAsync(string location, string property, GetPropertyCompletionMethod cm)
        {
            Uri uri = new Uri(GetUrlFromSysUri(location) + "?f??" + property);
            HttpHelper helper = new HttpHelper(uri, "POST", null, true, null);
            Debug.WriteLine("GetPropertyAsync: url = <" + uri.ToString() + ">");
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
            GetPropertyAsync("sys://Home", "DisplayName", (e) => 
                {
                    if (ConnectComplete != null)
                        ConnectComplete(this, new ConnectCompleteEventArgs(e.Response, e.Succeeded));
                });

        }

    }

}