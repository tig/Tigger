using GalaSoft.MvvmLight;
using Premise.Model;

namespace Premise.ViewModel
{
    /// <summary>
    /// This class contains properties that a View can data bind to.
    /// <para>
    /// Use the <strong>mvvminpc</strong> snippet to add bindable properties to this ViewModel.
    /// </para>
    /// <para>
    /// You can also use Blend to data bind with the tool's support.
    /// </para>
    /// <para>
    /// See http://www.galasoft.ch/mvvm/getstarted
    /// </para>
    /// </summary>
    public class SettingsViewModelBase : ViewModelBase
    {
        private Settings Settings = new Settings();

        /// <summary>
        /// Initializes a new instance of the Settings class.
        /// </summary>
        public SettingsViewModelBase()
        {
            PageName = "Base Settings";
            Description = "Nothing";

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
        /// Path the the view
        /// </summary>
        public string Page = "";

        /// <summary>
        /// The <see cref="Description" /> property's name.
        /// </summary>
        public const string DescriptionPropertyName = "Description";

        /// <summary>
        /// Gets the Description property.
        /// TODO Update documentation:
        /// Changes to that property's value raise the PropertyChanged event. 
        /// This property's value is broadcasted by the Messenger's default instance when it changes.
        /// </summary>
        public string Description
        {
            get
            {
                return Settings.Description;
            }

            set
            {
                if (Settings.Description == value)
                {
                    return;
                }

                var oldValue = Settings.Description;
                Settings.Description = value;

                // Update bindings, no broadcast
                RaisePropertyChanged(DescriptionPropertyName);

                // Update bindings and broadcast change using GalaSoft.MvvmLight.Messenging
//                RaisePropertyChanged(DescriptionPropertyName, oldValue, value, true);
            }
        }

        /// <summary>
        /// The <see cref="PageName" /> property's name.
        /// </summary>
        public const string PageNamePropertyName = "PageName";

        /// <summary>
        /// Gets the PageName property.
        /// TODO Update documentation:
        /// Changes to that property's value raise the PropertyChanged event. 
        /// This property's value is broadcasted by the Messenger's default instance when it changes.
        /// </summary>
        public string PageName
        {
            get
            {
                return Settings.PageName;
            }

            set
            {
                if (Settings.PageName == value)
                {
                    return;
                }

                var oldValue = Settings.PageName;
                Settings.PageName = value;

                // Update bindings, no broadcast
                RaisePropertyChanged(PageNamePropertyName);
            }
        }

        ////public override void Cleanup()
        ////{
        ////    // Clean own resources if needed

        ////    base.Cleanup();
        ////}
    }
}