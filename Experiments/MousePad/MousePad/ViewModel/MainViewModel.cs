// Copyright 2012 Charlie Kindel
//   
// This file is part of MousePad
//   

using System;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading;
using System.Windows.Threading;
using GalaSoft.MvvmLight;
using GalaSoft.MvvmLight.Threading;

namespace MousePad.ViewModel {
    /// <summary>
    ///   This class contains properties that the main View can data bind to.
    ///   <para> Use the <strong>mvvminpc</strong> snippet to add bindable properties to this ViewModel. </para>
    ///   <para> You can also use Blend to data bind with the tool's support. </para>
    ///   <para> See http://www.galasoft.ch/mvvm/getstarted </para>
    /// </summary>
    public class MainViewModel : ViewModelBase {
        /// <summary>
        ///   The <see cref="Host" /> property's name.
        /// </summary>
        public const string HostPropertyName = "Host";

        /// <summary>
        ///   The <see cref="Port" /> property's name.
        /// </summary>
        public const string PortPropertyName = "Port";

        /// <summary>
        ///   The <see cref="Connected" /> property's name.
        /// </summary>
        public const string ConnectedPropertyName = "Connected";


        /// <summary>
        ///   The <see cref="ProgressIndicatorVisible" /> property's name.
        /// </summary>
        public const string ProgressIndicatorVisibilePropertyName = "ProgressIndicatorVisible";

        /// <summary>
        ///   The <see cref="Error" /> property's name.
        /// </summary>
        public const string ErrorPropertyName = "Error";

        private bool _progressIndicatorVisible;
        private bool _connected;
        private Socket _connection;

        private String _error = "Not connected";
        private string _host = "chugach.kindel.net";
        private int _port = 5150;

        /// <summary>
        ///   Initializes a new instance of the MainViewModel class.
        /// </summary>
        public MainViewModel() {
            if (IsInDesignMode) {
                // Code runs in Blend --> create design time data.
            }
            else {
                // Code runs "for real"
            }
        }

        public string ApplicationTitle {
            get { return "MousePad"; }
        }

        public string PageName {
            get { return "Mouse Pad"; }
        }

        public string Host {
            get { return _host; }

            set {
                if (_host == value)
                    return;
                _host = value;
                // Update bindings, no broadcast
                DispatcherHelper.CheckBeginInvokeOnUI(() => RaisePropertyChanged(HostPropertyName));
            }
        }

        public int Port {
            get { return _port; }

            set {
                if (_port == value)
                    return;
                _port = value;
                // Update bindings, no broadcast
                DispatcherHelper.CheckBeginInvokeOnUI(() => RaisePropertyChanged(PortPropertyName));
            }
        }

        public bool Connected {
            get { return _connected; }

            set {
                if (_connected == value)
                    return;
                _connected = value;

                // Update bindings, no broadcast
                DispatcherHelper.CheckBeginInvokeOnUI(() => RaisePropertyChanged(ConnectedPropertyName));
            }
        }

        public String Error {
            get { return _error; }

            set {
                if (_error == value)
                    return;

                _error = value;
                // Update bindings, no broadcast
                DispatcherHelper.CheckBeginInvokeOnUI(() => RaisePropertyChanged(ErrorPropertyName));
            }
        }

        public bool ProgressIndicatorVisible
        {
            get { return _progressIndicatorVisible; }

            set
            {
                if (_progressIndicatorVisible == value)
                    return;

                _progressIndicatorVisible = value;
                // Update bindings, no broadcast
                DispatcherHelper.CheckBeginInvokeOnUI(() => RaisePropertyChanged(ProgressIndicatorVisibilePropertyName));
            }
        }

        public void Connect() {
            if (ProgressIndicatorVisible) return;

            ProgressIndicatorVisible = true;
            Error = "Connecting...";
            if (_connection == null)
            {
                _connection = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
            }

            var connectionOperation = new SocketAsyncEventArgs {RemoteEndPoint = new DnsEndPoint(Host, Port)};
            connectionOperation.Completed += (conn, connArgs) => {
                if (connArgs.SocketError == SocketError.Success){
                    //var testSend = new SocketAsyncEventArgs();
                    //var buffer = Encoding.UTF8.GetBytes("hello" + Environment.NewLine);
                    //testSend.SetBuffer(buffer, 0, buffer.Length);
                    //testSend.Completed += (s, args) =>{
                    //    Connected = (args.SocketError == SocketError.Success);
                    //    Error = args.SocketError.ToString();
                    //    ProgressIndicatorVisible = false;
                    //};
                    //_connection.SendAsync(testSend);

                    Connected = true;
                    Error = "Connected";
                    ProgressIndicatorVisible = false;
                }
                else {
                    Connected = false;
                    Error = connArgs.SocketError.ToString();
                    ProgressIndicatorVisible = false;
                }
            };

            _connection.ConnectAsync(connectionOperation);
        }

        public void Disconnect() {
            ProgressIndicatorVisible = false;
            Error = "Disconnected";
            Connected = false;
            if (_connection == null) return;
            _connection.Close();
            _connection = null;
        }

        public void SendMouseCmd(string action, string ps) {
            if (Connected) {
                var asyncEvent = new SocketAsyncEventArgs();
                asyncEvent.Completed += (sender, args) => {
                    Connected = (args.SocketError == SocketError.Success);
                    Error = args.SocketError.ToString();
                };

                String cmd;
                if (ps != null)
                    cmd = String.Format("mouse:{0},{1}{2}", action, ps, Environment.NewLine);
                else
                    cmd = String.Format("mouse:{0}{1}", action, Environment.NewLine);
                var buffer = Encoding.UTF8.GetBytes(cmd);
                asyncEvent.SetBuffer(buffer, 0, buffer.Length);

                _connection.SendAsync(asyncEvent);
            }
        }


        ////public override void Cleanup()
        ////{
        ////    // Clean up if needed

        ////    base.Cleanup();
        ////}
    }
}