using System;
using System.ComponentModel;
using System.Collections.Generic;
using System.Diagnostics;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Shapes;
using System.Collections.ObjectModel;
using GalaSoft.MvvmLight;
using GalaSoft.MvvmLight.Command;
using System.Reflection;
using Premise;
using Premise.Model;
using Microsoft.Phone.Reactive;

namespace Premise.ViewModel
{

    public class SecurityPanelViewModel : ViewModelBase
    {
        public PremiseServer Server;

        public SecurityPanelViewModel()
        {
            Debug.WriteLine("SecurityPanelViewModel");
            if (IsInDesignMode)
            {
                // Code runs in Blend --> create design time data.
            }
            else
            {
                // Code runs "for real": Connect to service, etc...

                ApplicationTitle = "Kindel Residence";

                KeyPadButtonCommand = new RelayCommand<String>((num) =>
                {
                    Debug.WriteLine("KeyPadButtonCommand {0}", num.ToString());
                    String code = Code;

                    // FIFO so it's always 4 numbers
                    if (code.Length == 4)
                        code = code.Substring(1, 3);

                    Code = code + num;
                });

                DisarmCommand = new RelayCommand(() =>
                {
                    Debug.WriteLine("DisarmCommand");
                    SecuirtySystemStatus.DisarmCode = Code;

                    
                });

                StayArmCommand = new RelayCommand(() =>
                {
                    Debug.WriteLine("StayArmCommand");
                    SecuirtySystemStatus.StayArm = true;
                });

                AwayArmCommand = new RelayCommand(() =>
                {
                    Debug.WriteLine("AwayArmCommand");
                    SecuirtySystemStatus.ExitArm = true;
                });
            }
        }

        // Commands
        #region Command properties
        public RelayCommand<String> KeyPadButtonCommand { get; private set; }
        public RelayCommand DisarmCommand { get; private set; }
        public RelayCommand StayArmCommand { get; private set; }
        public RelayCommand AwayArmCommand { get; private set; }
        #endregion



        public const string ApplicationTitlePropertyName = "ApplicationTitle";
        private string _ApplicationTitle;
        /// <summary>
        /// ApplicationTitle property; this property is used in the view to display its value using a Binding
        /// </summary>
        /// <returns></returns>
        public string ApplicationTitle
        {
            get
            {
                return _ApplicationTitle;
            }
            set
            {
                if (value != _ApplicationTitle)
                {
                    _ApplicationTitle = value;
                    RaisePropertyChanged(ApplicationTitlePropertyName);
                }
            }
        }

        public string PageName
        {
            get
            {
                return "Security Panel";
            }

        }

        /// <summary>
        /// The <see cref="SecuirtyQuickItem" /> property's name.
        /// </summary>
        public const string SecuirtyQuickItemPropertyName = "SecuirtyQuickItem";

        private SecurityQuickItem _SecurityQuickItem = null;

        /// <summary>
        /// Gets the SecuirtyQuickItem property.
        /// TODO Update documentation:
        /// Changes to that property's value raise the PropertyChanged event. 
        /// This property's value is broadcasted by the Messenger's default instance when it changes.
        /// </summary>
        public SecurityQuickItem SecuirtySystemStatus
        {
            get
            {
                return _SecurityQuickItem;
            }

            set
            {
                /*
                if (_SecurityQuickItem == value)
                {
                    return;
                }

                var oldValue = _SecurityQuickItem;
                _SecurityQuickItem = value;


                // Update bindings, no sendToServer
                RaisePropertyChanged(SecuirtyQuickItemPropertyName);
                 * */
            }
        }

        /// <summary>
        /// The <see cref="Zones" /> property's name.
        /// </summary>
        public const string ZonesPropertyName = "Zones";

        private PremiseObject _Zones = null;

        /// <summary>
        /// Gets the Zones property.
        /// TODO Update documentation:
        /// Changes to that property's value raise the PropertyChanged event. 
        /// This property's value is broadcasted by the Messenger's default instance when it changes.
        /// </summary>
        public PremiseObject Zones
        {
            get
            {
                return _Zones;
            }

            set
            {
                if (_Zones == value)
                {
                    return;
                }

                var oldValue = _Zones;
                _Zones = value;

                // Update bindings and sendToServer change using GalaSoft.MvvmLight.Messenging
                RaisePropertyChanged(ZonesPropertyName, oldValue, value, false);
            }
        }

        /// <summary>
        /// The <see cref="Code" /> property's name.
        /// </summary>
        public const string CodePropertyName = "Code";

        private String _Code = "";

        /// <summary>
        /// Code is the disarm code. It is only sent to the server upon an enter.
        /// </summary>
        public String Code
        {
            get
            {
                return _Code;
            }

            set
            {
                if (_Code == value)
                {
                    return;
                }

                var oldValue = _Code;
                _Code = value;

                RaisePropertyChanged(CodePropertyName, oldValue, value, false);
            }
        }


        private List<PremiseObject> objectList = new List<PremiseObject>();
       
        /// <summary>
        /// Creates and adds a few SecurityPanelensor objects into the Server object.
        /// </summary>
        /// 
        public void InitServerObjects()
        {
            Debug.WriteLine("SecurityPanelViewModel.InitServerObjects");
            // Sample data; replace with real data

            Server = PremiseServer.Instance;

            // Invoke connect async method. When our delegate is called the connection attempt 
            // is complete.  It's either a success (Response is the DisplayName of Home) or a failure 
            // (Response is the error).
            //
            Server.ConnectComplete += (o, ea) =>
            {
                if (ea.Succeeded)
                {
                    GalaSoft.MvvmLight.Threading.DispatcherHelper.CheckBeginInvokeOnUI(() =>
                    {
                        ApplicationTitle = ea.Response.ToString();
                        Update();
                    });
                }
                else
                    GalaSoft.MvvmLight.Threading.DispatcherHelper.CheckBeginInvokeOnUI(() => ApplicationTitle = "No Connection: " + ea.Response.ToString());
            };

            _SecurityQuickItem = (SecurityQuickItem)Server.GetOrCreateServerObject("SecurityQuickItem", "sys://Home/Security");
            objectList.Add(_SecurityQuickItem);
            _Zones = (PremiseObject)Server.GetOrCreateServerObject("PremiseObject", "sys://Devices/CADDX/NX8e_Panel/Zones/Zone_6");
            objectList.Add(_Zones);
        }

        public void Update()
        {
            Server.QueryObects(objectList);
        }
    }
}