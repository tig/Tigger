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
using GalaSoft.MvvmLight.Threading;
using GalaSoft.MvvmLight.Messaging;
using System.Reflection;
using Premise;
using Premise.Model;

namespace Premise.ViewModel
{

    public class AllHouseLightingViewModel : ViewModelBase
    {
        public PremiseServer Server;

        public AllHouseLightingViewModel()
        {
            Debug.WriteLine("AllHouseLightingViewModel");
            if (IsInDesignMode)
            {
                // Code runs in Blend --> create design time data.
            }
            else
            {
                // Code runs "for real": Connect to service, etc...

                ApplicationTitle = "";

                // Create our server instance. It will not talk on the network until we tell it to (Server.Connect).
                //if (ViewModelLocator.MainViewModelStatic.Server == null)
                //    ViewModelLocator.MainViewModelStatic.Server = new PremiseServer("home.kindel.net", 80);

            }
        }

        /// <summary>
        /// The <see cref="SelectedItem" /> property's name.
        /// </summary>
        public const string SelectedItemPropertyName = "SelectedItem";

        private LightingScene _SelectedItem = null;

        /// <summary>
        /// Gets the SelectedItem property: Used to handle ListBox selections.
        /// TODO Update documentation:
        /// Changes to that property's value raise the PropertyChanged event. 
        /// This property's value is broadcasted by the Messenger's default instance when it changes.
        /// </summary>
        public PremiseObject SelectedItem
        {
            get
            {
                return _SelectedItem;
            }

            set
            {
                if (_SelectedItem == value)
                {
                    return;
                }

                //                Debug.WriteLine("SelectedItem set {1}", value.ToString());
                var oldValue = _SelectedItem;
                _SelectedItem = (LightingScene)value;

                //if (_SelectedItem.GetType() == typeof(GDOQuickItem))
                //                     NavigationService.Navigate(new Uri("/GarageDoorsPage.xaml", UriKind.Relative));


                // Update bindings, no sendToServer
                //RaisePropertyChanged(SelectedItemPropertyName);

                // Update bindings and sendToServer change using GalaSoft.MvvmLight.Messenging
                //RaisePropertyChanged(SelectedItemPropertyName, oldValue, value, true);

                if (_SelectedItem != null)
                    _SelectedItem.Trigger = true;
            }
        }

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
                return "Lighting Scenes";
            }

        }

       
        /// <summary>
        /// A collection for LightingScene objects.
        /// </summary>
        public ObservableCollection<LightingScene> Items { get; private set; }
        /// <summary>
        /// Creates and adds a few LightingScene objects into the Server object.
        /// </summary>
        public void InitServerObjects()
        {
            Debug.WriteLine("AllHouseLightingViewModel.InitServerObjects");

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
                        this.Update();
                    });
                }
                else
                    DispatcherHelper.CheckBeginInvokeOnUI(() => ApplicationTitle = "No Connection: " + ea.Response.ToString());
            };


            Items = new ObservableCollection<LightingScene>();

//            Items.Add((LightingScene)Server.GetOrCreateServerObject("LightingScene", "sys://Home/Admin/All House Scenes/Interior Lights Off"));
//            Items.Add((LightingScene)Server.GetOrCreateServerObject("LightingScene", "sys://Home/Admin/All House Scenes/Exterior Lights Off"));
//            Items.Add((LightingScene)Server.GetOrCreateServerObject("LightingScene", "sys://Home/Admin/All House Scenes/Emergency Lighting On"));
//            Items.Add((LightingScene)Server.GetOrCreateServerObject("LightingScene", "sys://Home/Admin/All House Scenes/Night Scene"));
//            Items.Add((LightingScene)Server.GetOrCreateServerObject("LightingScene", "sys://Home/Admin/All House Scenes/Night Pathway Scene"));
            
            // Gotta keep track of all PremiseObjects on this view so they get updated
            // by the timer.
//            foreach (LightingScene item in Items)
//            {
                //objectList.Add((PremiseObject)item);
//            }

        }

        private List<PremiseObject> objectList = new List<PremiseObject>();
        public void Update()
        {
            if (Items.Count == 0)
            {
                Server.InvokeMethodAsync("sys://Home/Admin/All House Scenes", "GetButtons()", (HttpResponseArgs) =>
                {
                    if (HttpResponseArgs.Succeeded)
                    {
                        //Debug.WriteLine("Received {0}: {1} = {2}", DisplayName, propertyName, HttpResponseArgs.Response);
                        DispatcherHelper.CheckBeginInvokeOnUI(() =>
                        {
                            String strScenes = (String)HttpResponseArgs.Response; // <-- this is the whole cause of this confusing architecture
                            foreach (String s in strScenes.Split(','))
                            {
                                if (s.Length > 0)
                                {
                                    //Debug.WriteLine("Scene: {0}", s);
                                    PremiseObject o = Server.GetOrCreateServerObject("LightingScene", s);
                                    Items.Add((LightingScene)o);
                                    o.QueryServer(Server);  // Manually query each item when created
                                    objectList.Add(o);
                                }
                           }
                        });
                    }
                });
            }
            else
                Server.QueryObects(objectList);
        }
    }
}