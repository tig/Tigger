using System.Diagnostics;
/*
  In App.xaml:
  <Application.Resources>
      <vm:ViewModelLocatorTemplate xmlns:vm="clr-namespace:Premise.ViewModel"
                                   x:Key="Locator" />
  </Application.Resources>
  
  In the View:
  DataContext="{Binding Source={StaticResource Locator}, Path=ViewModelName}"
  
  OR (WPF only):
  
  xmlns:vm="clr-namespace:Premise.ViewModel"
  DataContext="{Binding Source={x:Static vm:ViewModelLocatorTemplate.ViewModelNameStatic}}"
*/

namespace Premise.ViewModel
{
    /// <summary>
    /// This class contains static references to all the view models in the
    /// application and provides an entry point for the bindings.
    /// <para>
    /// Use the <strong>mvvmlocatorproperty</strong> snippet to add ViewModels
    /// to this locator.
    /// </para>
    /// <para>
    /// In Silverlight and WPF, place the ViewModelLocatorTemplate in the App.xaml resources:
    /// </para>
    /// <code>
    /// &lt;Application.Resources&gt;
    ///     &lt;vm:ViewModelLocatorTemplate xmlns:vm="clr-namespace:Premise.ViewModel"
    ///                                  x:Key="Locator" /&gt;
    /// &lt;/Application.Resources&gt;
    /// </code>
    /// <para>
    /// Then use:
    /// </para>
    /// <code>
    /// DataContext="{Binding Source={StaticResource Locator}, Path=ViewModelName}"
    /// </code>
    /// <para>
    /// You can also use Blend to do all this with the tool's support.
    /// </para>
    /// <para>
    /// See http://www.galasoft.ch/mvvm/getstarted
    /// </para>
    /// <para>
    /// In <strong>*WPF only*</strong> (and if databinding in Blend is not relevant), you can delete
    /// the Main property and bind to the ViewModelNameStatic property instead:
    /// </para>
    /// <code>
    /// xmlns:vm="clr-namespace:Premise.ViewModel"
    /// DataContext="{Binding Source={x:Static vm:ViewModelLocatorTemplate.ViewModelNameStatic}}"
    /// </code>
    /// </summary>
    public class ViewModelLocator
    {
        private static MainViewModel _mainViewModel;

        /// <summary>
        /// Initializes a new instance of the ViewModelLocator class.
        /// </summary>
        public ViewModelLocator()
        {
            ////if (ViewModelBase.IsInDesignModeStatic)
            ////{
            ////    // Create design time view models
            ////}
            ////else
            ////{
            ////    // Create run time view models
            ////}

            CreateMainViewModel();
            CreateGarageDoorSensorViewModel();
            CreateItemViewModel();
            CreateGarageDoorsViewModel();
            CreateSecurityPanelViewModel();
            CreateAllHouseLightingViewModel();
            CreateIrrigationViewModel();
            CreateSettingsViewModelBase();
            CreateConnectionSettingsViewModel();
        }

        #region MainViewModel

        /// <summary>
        /// Gets the Main property.
        /// </summary>
        public static MainViewModel MainViewModelStatic
        {
            get
            {
                if (_mainViewModel == null)
                {
                    CreateMainViewModel();
                }

                return _mainViewModel;
            }
        }

        /// <summary>
        /// Gets the Main property.
        /// </summary>
        [System.Diagnostics.CodeAnalysis.SuppressMessage("Microsoft.Performance",
            "CA1822:MarkMembersAsStatic",
            Justification = "This non-static member is needed for data binding purposes.")]
        public MainViewModel MainViewModel
        {
            get
            {
                return MainViewModelStatic;
            }
        }

        /// <summary>
        /// Provides a deterministic way to delete the Main property.
        /// </summary>
        public static void ClearMainViewModel()
        {
            _mainViewModel.Cleanup();
            _mainViewModel = null;
        }

        /// <summary>
        /// Provides a deterministic way to create the Main property.
        /// </summary>
        public static void CreateMainViewModel()
        {
            if (_mainViewModel == null)
            {
                _mainViewModel = new MainViewModel();
            }
        }
#endregion

        #region GarageDoorSensorViewModel

        private static GarageDoorSensorViewModel _gdoViewModel;
        /// <summary>
        /// Gets the GarageDoorSensorViewModel property.
        /// </summary>
        public static GarageDoorSensorViewModel GarageDoorSensorViewModelStatic
        {
            get
            {
                if (_gdoViewModel == null)
                {
                    CreateGarageDoorSensorViewModel();
                }

                return _gdoViewModel;
            }
        }

        /// <summary>
        /// Gets the Main property.
        /// </summary>
        [System.Diagnostics.CodeAnalysis.SuppressMessage("Microsoft.Performance",
            "CA1822:MarkMembersAsStatic",
            Justification = "This non-static member is needed for data binding purposes.")]
        public GarageDoorSensorViewModel GarageDoorSensorViewModel
        {
            get
            {
                return GarageDoorSensorViewModelStatic;
            }
        }

        /// <summary>
        /// Provides a deterministic way to delete the Main property.
        /// </summary>
        public static void ClearGarageDoorSensorViewModel()
        {
            if (_gdoViewModel != null)
                _gdoViewModel.Cleanup();
            _gdoViewModel = null;
        }

        /// <summary>
        /// Provides a deterministic way to create the Main property.
        /// </summary>
        public static void CreateGarageDoorSensorViewModel()
        {
            if (_gdoViewModel == null)
            {
                _gdoViewModel = new GarageDoorSensorViewModel();
            }
        }
        #endregion

        #region QuickItemViewModel
        private static QuickItemViewModel _item;

        /// <summary>
        /// Gets the QuickItemViewModel property.
        /// </summary>
        public static QuickItemViewModel ItemViewModelStatic
        {
            get
            {
                if (_item == null)
                {
                    CreateItemViewModel();
                }

                return _item;
            }
        }

        /// <summary>
        /// Gets the QuickItemViewModel property.
        /// </summary>
        [System.Diagnostics.CodeAnalysis.SuppressMessage("Microsoft.Performance",
            "CA1822:MarkMembersAsStatic",
            Justification = "This non-static member is needed for data binding purposes.")]
        public QuickItemViewModel ItemViewModel
        {
            get
            {
                return ItemViewModelStatic;
            }
        }

        /// <summary>
        /// Provides a deterministic way to delete the QuickItemViewModel property.
        /// </summary>
        public static void ClearItemViewModel()
        {
            if (_item != null)
                _item.Cleanup();
            _item = null;
        }

        /// <summary>
        /// Provides a deterministic way to create the QuickItemViewModel property.
        /// </summary>
        public static void CreateItemViewModel()
        {
            if (_item == null)
            {
                _item = new QuickItemViewModel();
            }
        }
        #endregion

        #region GarageDoorsViewModel
        private static GarageDoorsViewModel _garageDoors;

        /// <summary>
        /// Gets the GarageDoorsViewModel property.
        /// </summary>
        public static GarageDoorsViewModel GarageDoorsViewModelStatic
        {
            get
            {
                if (_garageDoors == null)
                {
                    CreateGarageDoorsViewModel();
                }

                return _garageDoors;
            }
        }

        /// <summary>
        /// Gets the GarageDoorsViewModel property.
        /// </summary>
        [System.Diagnostics.CodeAnalysis.SuppressMessage("Microsoft.Performance",
            "CA1822:MarkMembersAsStatic",
            Justification = "This non-static member is needed for data binding purposes.")]
        public GarageDoorsViewModel GarageDoorsViewModel
        {
            get
            {
                return GarageDoorsViewModelStatic;
            }
        }

        /// <summary>
        /// Provides a deterministic way to delete the GarageDoorsViewModel property.
        /// </summary>
        public static void ClearGarageDoorsViewModel()
        {
            if (_garageDoors != null)
                _garageDoors.Cleanup();
            _garageDoors = null;
        }

        /// <summary>
        /// Provides a deterministic way to create the GarageDoorsViewModel property.
        /// </summary>
        public static void CreateGarageDoorsViewModel()
        {
            if (_garageDoors == null)
            {
                _garageDoors = new GarageDoorsViewModel();
            }
        }
        #endregion

        #region SecurityPanelViewModel
        private static SecurityPanelViewModel _SecurityPanel;

        /// <summary>
        /// Gets the SecurityPanelViewModel property.
        /// </summary>
        public static SecurityPanelViewModel SecurityPanelViewModelStatic
        {
            get
            {
                if (_SecurityPanel == null)
                {
                    CreateSecurityPanelViewModel();
                }

                return _SecurityPanel;
            }
        }

        /// <summary>
        /// Gets the SecurityPanelViewModel property.
        /// </summary>
        [System.Diagnostics.CodeAnalysis.SuppressMessage("Microsoft.Performance",
            "CA1822:MarkMembersAsStatic",
            Justification = "This non-static member is needed for data binding purposes.")]
        public SecurityPanelViewModel SecurityPanelViewModel
        {
            get
            {
                return SecurityPanelViewModelStatic;
            }
        }

        /// <summary>
        /// Provides a deterministic way to delete the SecurityPanelViewModel property.
        /// </summary>
        public static void ClearSecurityPanelViewModel()
        {
            if (_SecurityPanel != null)
                _SecurityPanel.Cleanup();
            _SecurityPanel = null;
        }

        /// <summary>
        /// Provides a deterministic way to create the SecurityPanelViewModel property.
        /// </summary>
        public static void CreateSecurityPanelViewModel()
        {
            if (_SecurityPanel == null)
            {
                _SecurityPanel = new SecurityPanelViewModel();
            }
        }
        #endregion
        
        #region AllHouseLightingViewModel
        private static AllHouseLightingViewModel _AllHouseLighting;

        /// <summary>
        /// Gets the AllHouseLightingViewModel property.
        /// </summary>
        public static AllHouseLightingViewModel AllHouseLightingViewModelStatic
        {
            get
            {
                if (_AllHouseLighting == null)
                {
                    CreateAllHouseLightingViewModel();
                }

                return _AllHouseLighting;
            }
        }

        /// <summary>
        /// Gets the AllHouseLightingViewModel property.
        /// </summary>
        [System.Diagnostics.CodeAnalysis.SuppressMessage("Microsoft.Performance",
            "CA1822:MarkMembersAsStatic",
            Justification = "This non-static member is needed for data binding purposes.")]
        public AllHouseLightingViewModel AllHouseLightingViewModel
        {
            get
            {
                return AllHouseLightingViewModelStatic;
            }
        }

        /// <summary>
        /// Provides a deterministic way to delete the AllHouseLightingViewModel property.
        /// </summary>
        public static void ClearAllHouseLightingViewModel()
        {
            if (_AllHouseLighting != null)
                _AllHouseLighting.Cleanup();
            _AllHouseLighting = null;
        }

        /// <summary>
        /// Provides a deterministic way to create the AllHouseLightingViewModel property.
        /// </summary>
        public static void CreateAllHouseLightingViewModel()
        {
            if (_AllHouseLighting == null)
            {
                _AllHouseLighting = new AllHouseLightingViewModel();
            }
        }
        #endregion

        #region IrrigationViewModel
        private static IrrigationViewModel _Irrigation;

        /// <summary>
        /// Gets the IrrigationViewModel property.
        /// </summary>
        public static IrrigationViewModel IrrigationViewModelStatic
        {
            get
            {
                if (_Irrigation == null)
                {
                    CreateIrrigationViewModel();
                }

                return _Irrigation;
            }
        }

        /// <summary>
        /// Gets the IrrigationViewModel property.
        /// </summary>
        [System.Diagnostics.CodeAnalysis.SuppressMessage("Microsoft.Performance",
            "CA1822:MarkMembersAsStatic",
            Justification = "This non-static member is needed for data binding purposes.")]
        public IrrigationViewModel IrrigationViewModel
        {
            get
            {
                return IrrigationViewModelStatic;
            }
        }

        /// <summary>
        /// Provides a deterministic way to delete the IrrigationViewModel property.
        /// </summary>
        public static void ClearIrrigationViewModel()
        {
            if (_Irrigation != null)
                _Irrigation.Cleanup();
            _Irrigation = null;
        }

        /// <summary>
        /// Provides a deterministic way to create the IrrigationViewModel property.
        /// </summary>
        public static void CreateIrrigationViewModel()
        {
            if (_Irrigation == null)
            {
                _Irrigation = new IrrigationViewModel();
            }
        }
        #endregion

        #region SettingsViewModelBase
        private static SettingsViewModelBase _SettingsViewModelBase;

        /// <summary>
        /// Gets the SettingsViewModelBase property.
        /// </summary>
        public static SettingsViewModelBase SettingsViewModelBaseStatic
        {
            get
            {
                if (_SettingsViewModelBase == null)
                {
                    CreateSettingsViewModelBase();
                }

                return _SettingsViewModelBase;
            }
        }

        /// <summary>
        /// Gets the SettingsViewModelBase property.
        /// </summary>
        [System.Diagnostics.CodeAnalysis.SuppressMessage("Microsoft.Performance",
            "CA1822:MarkMembersAsStatic",
            Justification = "This non-static member is needed for data binding purposes.")]
        public SettingsViewModelBase SettingsViewModelBase
        {
            get
            {
                return SettingsViewModelBaseStatic;
            }
        }

        /// <summary>
        /// Provides a deterministic way to delete the SettingsViewModelBase property.
        /// </summary>
        public static void ClearSettingsViewModelBase()
        {
            if (_SettingsViewModelBase != null)
                _SettingsViewModelBase.Cleanup();
            _SettingsViewModelBase = null;
        }

        /// <summary>
        /// Provides a deterministic way to create the SettingsViewModelBase property.
        /// </summary>
        public static void CreateSettingsViewModelBase()
        {
            if (_SettingsViewModelBase == null)
            {
                _SettingsViewModelBase = new SettingsViewModelBase();
            }
        }
        #endregion

        #region ConnectionSettingsViewModel
        private static ConnectionSettingsViewModel _ConnectionSettingsViewModel;

        /// <summary>
        /// Gets the ConnectionSettingsViewModel property.
        /// </summary>
        public static ConnectionSettingsViewModel ConnectionSettingsViewModelStatic
        {
            get
            {
                if (_ConnectionSettingsViewModel == null)
                {
                    CreateConnectionSettingsViewModel();
                }

                return _ConnectionSettingsViewModel;
            }
        }

        /// <summary>
        /// Gets the ConnectionSettingsViewModel property.
        /// </summary>
        [System.Diagnostics.CodeAnalysis.SuppressMessage("Microsoft.Performance",
            "CA1822:MarkMembersAsStatic",
            Justification = "This non-static member is needed for data binding purposes.")]
        public ConnectionSettingsViewModel ConnectionSettingsViewModel
        {
            get
            {
                return ConnectionSettingsViewModelStatic;
            }
        }

        /// <summary>
        /// Provides a deterministic way to delete the ConnectionSettingsViewModel property.
        /// </summary>
        public static void ClearConnectionSettingsViewModel()
        {
            Debug.WriteLine("ClearConnectionSettingsViewModel");
            if (_ConnectionSettingsViewModel != null)
                _ConnectionSettingsViewModel.Cleanup();
            _ConnectionSettingsViewModel = null;
        }

        /// <summary>
        /// Provides a deterministic way to create the ConnectionSettingsViewModel property.
        /// </summary>
        public static void CreateConnectionSettingsViewModel()
        {
            if (_ConnectionSettingsViewModel == null)
            {
                _ConnectionSettingsViewModel = new ConnectionSettingsViewModel();
            }
        }
        #endregion
        /// <summary>
        /// Cleans up all the resources.
        /// </summary>
        public static void Cleanup()
        {
            ClearMainViewModel();
            ClearGarageDoorSensorViewModel();
            ClearItemViewModel();
            ClearGarageDoorsViewModel();
            ClearSecurityPanelViewModel();
            ClearAllHouseLightingViewModel();
            ClearIrrigationViewModel();
            ClearSettingsViewModelBase();
            ClearConnectionSettingsViewModel();
        }
    }
}