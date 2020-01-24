using System.Diagnostics;
using GalaSoft.MvvmLight;
using Premise.Model;


namespace Premise.ViewModel
{
    /// <summary>
    /// The settings view for Connection Settings binds to this VM.
    /// This VM talks directly to the Server instance held by MainViewModel.
    /// </summary>
    public class ConnectionSettingsViewModel : SettingsViewModelBase
    {
        /// <summary>
        /// Initializes a new instance of the ConnectionSettingsViewModel class.
        /// </summary>
        public ConnectionSettingsViewModel()
        {
            Debug.WriteLine("ConnectionSettingsViewModel()");
            PageName = "Connection Settings";
            Description = "Settings for the server connection";

            ////if (IsInDesignMode)
            ////{
            ////    // Code runs in Blend --> create design time data.
            ////}
            ////else
            ////{
            ////    // Code runs "for real": Connect to service, etc...
            ////}
        }

        /// <summary>
        /// The <see cref="Host" /> property's name.
        /// </summary>
        public const string HostPropertyName = "Host";

        /// <summary>
        /// Gets the Host property.
        /// TODO Update documentation:
        /// Changes to that property's value raise the PropertyChanged event. 
        /// This property's value is broadcasted by the Messenger's default instance when it changes.
        /// </summary>
        public string Host
        {
            get
            {
                return PremiseServer.Instance.Host;
            }

            set
            {
                if (PremiseServer.Instance.Host == value)
                {
                    return;
                }

                var oldValue = PremiseServer.Instance.Host;
                PremiseServer.Instance.Host = value;

                // Update bindings, no broadcast
                RaisePropertyChanged(HostPropertyName);
                Debug.WriteLine("Host: {0}", value);
            }
        }

        /// <summary>
        /// The <see cref="Port" /> property's name.
        /// </summary>
        public const string PortPropertyName = "Port";

        /// <summary>
        /// Gets the Port property.
        /// TODO Update documentation:
        /// Changes to that property's value raise the PropertyChanged event. 
        /// This property's value is broadcasted by the Messenger's default instance when it changes.
        /// </summary>
        public int Port
        {
            get
            {
                return PremiseServer.Instance.Port;
            }

            set
            {
                if (PremiseServer.Instance.Port == value)
                {
                    return;
                }

                var oldValue = PremiseServer.Instance.Port;
                PremiseServer.Instance.Port = value;

                // Update bindings, no broadcast
                RaisePropertyChanged(PortPropertyName);
                Debug.WriteLine("Port: {0}", value);

            }
        }

        /// <summary>
        /// The <see cref="SSL" /> property's name.
        /// </summary>
        public const string SSLPropertyName = "SSL";

        /// <summary>
        /// Gets the SSL property.
        /// TODO Update documentation:
        /// Changes to that property's value raise the PropertyChanged event. 
        /// This property's value is broadcasted by the Messenger's default instance when it changes.
        /// </summary>
        public bool SSL
        {
            get
            {
                return PremiseServer.Instance.SSL;
            }

            set
            {
                if (PremiseServer.Instance.SSL == value)
                {
                    return;
                }

                var oldValue = PremiseServer.Instance.SSL;
                PremiseServer.Instance.SSL = value;

                // Update bindings, no broadcast
                RaisePropertyChanged(SSLPropertyName);
                Debug.WriteLine("SSL: {0}", value);

            }
        }

        /// <summary>
        /// The <see cref="Username" /> property's name.
        /// </summary>
        public const string UsernamePropertyName = "Username";

        /// <summary>
        /// Gets the Username property.
        /// TODO Update documentation:
        /// Changes to that property's value raise the PropertyChanged event. 
        /// This property's value is broadcasted by the Messenger's default instance when it changes.
        /// </summary>
        public string Username
        {
            get
            {
                return PremiseServer.Instance.Username;
            }

            set
            {
                if (PremiseServer.Instance.Username == value)
                {
                    return;
                }

                var oldValue = PremiseServer.Instance.Username;
                PremiseServer.Instance.Username = value;

                // Update bindings, no broadcast
                RaisePropertyChanged(UsernamePropertyName);
                Debug.WriteLine("Username: {0}", value);

            }
        }

        /// <summary>
        /// The <see cref="Password" /> property's name.
        /// </summary>
        public const string PasswordPropertyName = "Password";

        /// <summary>
        /// Gets the Password property.
        /// TODO Update documentation:
        /// Changes to that property's value raise the PropertyChanged event. 
        /// This property's value is broadcasted by the Messenger's default instance when it changes.
        /// </summary>
        public string Password
        {
            get
            {
                return PremiseServer.Instance.Password;
            }

            set
            {
                if (PremiseServer.Instance.Password == value)
                {
                    return;
                }

                var oldValue = PremiseServer.Instance.Password;
                PremiseServer.Instance.Password = value;

                // Update bindings, no broadcast
                RaisePropertyChanged(PasswordPropertyName);
                Debug.WriteLine("Password: {0}", value);

            }
        }


        ////public override void Cleanup()
        ////{
        ////    // Clean own resources if needed

        ////    base.Cleanup();
        ////}
    }
}