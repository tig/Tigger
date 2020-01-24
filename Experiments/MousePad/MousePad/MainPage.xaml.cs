using System;
using System.Net;
using System.Net.Sockets;
using System.Windows;
using System.Windows.Data;
using System.Windows.Input;
using Microsoft.Phone.Controls;
using Microsoft.Xna.Framework.Input.Touch;
using MousePad.ViewModel;
using System.Text;
using GestureEventArgs = Microsoft.Phone.Controls.GestureEventArgs;


namespace MousePad
{
    public partial class MainPage : PhoneApplicationPage {
        private MainViewModel _vm;
        // Constructor
        public MainPage()
        {
            InitializeComponent();
            TouchPanel.EnabledGestures = GestureType.Tap | GestureType.DoubleTap | GestureType.Hold;

            _vm = ViewModelLocator.MainStatic;

        }

        private void padTap(object sender, System.Windows.Input.GestureEventArgs e)
        {
                _vm.SendMouseCmd("lbc", null);
        }

        private StylusPoint _startPoint;
        private void padMouseLeftButtonDown(object sender, System.Windows.Input.MouseButtonEventArgs e)
        {
            //SendMouseCmd("lbd", null);
            _startPoint = e.StylusDevice.GetStylusPoints(pad)[0];
        }

        private void padMouseLeftButtonUp(object sender, System.Windows.Input.MouseButtonEventArgs e)
        {
            //SendMouseCmd("lbu", null);
            var points = e.StylusDevice.GetStylusPoints(pad);
        }


        private static int _count;
        private void padMouseMove(object sender, System.Windows.Input.MouseEventArgs e)
        {
            var points = e.StylusDevice.GetStylusPoints(pad);

            var dx = _startPoint.X - points[0].X;
            var dy = _startPoint.Y - points[0].Y;

            _startPoint = points[0];

            _count++;
            if (_count > 0) {
                _vm.SendMouseCmd("mm", String.Format("{0},{1}", Math.Floor(-dx*1), Math.Floor(-dy*1)));
                _count = 0;
            }

        }

        private void padDoubleTap(object sender, System.Windows.Input.GestureEventArgs gestureEventArgs)
        {
            _vm.SendMouseCmd("lbdc", null);
        }

        private void connectClick(object sender, RoutedEventArgs e) {
            _vm.Connect();
        }

        private void PhoneApplicationPage_Loaded(object sender, RoutedEventArgs e)
        {

        }

        private void disconnectClick(object sender, EventArgs e)
        {
            _vm.Disconnect();
        }

        private void keyboard_Click(object sender, EventArgs e)
        {

        }

        private void settings_Click(object sender, EventArgs e)
        {

        }


    }


    public class VisibilityConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
        {
            if (value is bool?)
            {
                if (string.IsNullOrEmpty((string)parameter))
                {
                    return (value as bool?).Value ? Visibility.Visible : Visibility.Collapsed;
                }
                else
                {
                    return (value as bool?).Value ? Visibility.Collapsed : Visibility.Visible;
                }
            }
            throw new ArgumentException();
        }

        public object ConvertBack(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
        {
            throw new NotImplementedException();
        }
    }


    public class VisibilityInverseConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
        {
            if (value is bool?)
            {
                if (string.IsNullOrEmpty((string)parameter))
                {
                    return !(value as bool?).Value ? Visibility.Visible : Visibility.Collapsed;
                }
                else
                {
                    return !(value as bool?).Value ? Visibility.Collapsed : Visibility.Visible;
                }
            }
            throw new ArgumentException();
        }

        public object ConvertBack(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
        {
            throw new NotImplementedException();
        }
    }
}
