using System;
using System.Net;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Documents;
using System.Windows.Ink;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Animation;
using System.Windows.Shapes;
using System.Diagnostics;

namespace Premise
{
    //where T : string, bool, int, float 
    public class SysProperty<T> 
    {
        private T _Value;
        public SysProperty(String propertyName)
        {
            PropertyName = propertyName;
            UpdatedFromServer = false;
        }

        public T Value
        {
            get 
            { 
                return _Value; 
            }
            
            set 
            {
                _Value = value;
            }
        }

        public void QueryServer()
        {

        }

        private SysObject _Object;
        public SysObject Object 
        {
            get { return _Object; } 
            set
            {
                _Object = value;
            }
        }
        public String PropertyName { get; private set; }

        public bool UpdatedFromServer { get; set; }
    }
}
