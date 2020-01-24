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

namespace Premise.Model
{
    /// <summary>
    /// Represents a property on a premise object.
    /// 
    /// </summary>
    /// <typeparam name="T">string, bool, int, float </typeparam>
    public class PremiseProperty<T>  
    {
        private T _Value;
        public PremiseProperty(String propertyName, bool persistent = false, bool ignoreServer = false)
        {
            PropertyName = propertyName;
            UpdatedFromServer = false;
            Persistent = persistent;
            IgnoreServer = ignoreServer;
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

        /// <summary>
        /// The name of the property. Matches the name on the server.
        /// </summary>
        public String PropertyName { get; set; }

        /// <summary>
        /// If True then this property value should be persisted on the client during tombstoning
        /// and never refreshed from the server (e.g. it never changes on the server, so don't 
        /// ask for it again). 
        /// 
        /// TODO: For now we don't actually persist to storage, so this is really "cached"
        /// 
        /// </summary>
        public bool Persistent { get; set; }

        /// <summary>
        /// If True then this property value will never be retrieved from the server. This is useful
        /// for property values on the server we want to ingore on the client such as DisplayName's that
        /// change on the server for random reasons (e.g. IrrigationPro stores interal state in DisplayName that
        /// we don't care about).
        /// </summary>
        public bool IgnoreServer { get; set; }

        /// <summary>
        /// True if the property value was retrieved from the server.
        /// </summary>
        public bool UpdatedFromServer { get; set; }

        public bool CheckGetFromServer
        {
            get
            {
                if (IgnoreServer) return false;

                if (Persistent && UpdatedFromServer) return false;

                return true;
            }
        }
    }

}
