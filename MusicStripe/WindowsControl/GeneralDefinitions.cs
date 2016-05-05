using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading.Tasks;

namespace WindowsControl
{

    public class WifiAPEventArgs : EventArgs
    {
        private ESPWifiData _ESPData;

        public ESPWifiData ESPData
        {
            get
            {
                return _ESPData;
            }

            set
            {
                _ESPData = value;
            }
        }

        public WifiAPEventArgs(ESPWifiData ESPs)
        {

            _ESPData = ESPs;
        }
    }
    public struct ESPData
    {
        ESPWifiData _Adresse;
        bool _LCD;
        LEDHard[] _LEDs;
        public ESPData(ESPWifiData ConnectionData, bool hasLCD, LEDHard[] LEDData)
        {
            _Adresse = ConnectionData;
            _LCD = hasLCD;
            _LEDs = LEDData;
        }

        public ESPWifiData Adresse
        {
            get
            {
                return _Adresse;
            }

            set
            {
                _Adresse = value;
            }
        }

        public bool LCD
        {
            get
            {
                return _LCD;
            }

            set
            {
                _LCD = value;
            }
        }

        public LEDHard[] LEDs
        {
            get
            {
                return _LEDs;
            }

            set
            {
                _LEDs = value;
            }
        }
    }
    public struct LEDHard
    {
        byte[] _Pins;
        bool _Active;
        byte[] _Colors;
        byte _Frequenzy;
        byte _Mode;
        public byte[] Pins
        {
            get
            {
                return _Pins;
            }

            set
            {
                _Pins = value;
            }
        }

        public bool Active
        {
            get
            {
                return _Active;
            }

            set
            {
                _Active = value;
            }
        }

        public byte[] Colors
        {
            get
            {
                return _Colors;
            }

            set
            {
                _Colors = value;
            }
        }

        public byte Frequenzy
        {
            get
            {
                return _Frequenzy;
            }

            set
            {
                _Frequenzy = value;
            }
        }

        public byte Mode
        {
            get
            {
                return _Mode;
            }

            set
            {
                _Mode = value;
            }
        }

        public LEDHard(byte PinRed,
        byte PinGreen, byte PinBlue, bool active)
        {
            if (active)
                this = new LEDHard(PinRed, PinBlue, PinGreen, active, 255, 255, 255, 0, 0);
            else
                this = new LEDHard(PinRed, PinBlue, PinGreen, active, 0, 0, 0, 0, 0);

        }
        public LEDHard(byte[] Data) : this(Data[0], Data[1], Data[2], System.Convert.ToBoolean(Data[3]), Data[4], Data[5], Data[6], Data[7], Data[8]) { }
        //  public LEDHard(byte[] Data) : this(Data[0], Data[1], Data[2], Data[3], Data[4]) { };

        public LEDHard(byte PinRed,
       byte PinGreen, byte PinBlue, bool active, byte Red, byte Green, byte Blue, byte frequenzy, byte mode)
        {
            _Active = active;
            _Pins = new byte[] { PinRed, PinGreen, PinBlue };
            _Colors = new byte[] { Red, Green, Blue };
            _Frequenzy = frequenzy;
            _Mode = mode;

        }
    }
    public struct ESPWifiData
    {
        string _IPAdresse;
        int _LocalPort;

        public string IPAdresse
        {
            get
            {
                return _IPAdresse;
            }

            set
            {
                _IPAdresse = value;
            }
        }

        public int LocalPort
        {
            get
            {
                return _LocalPort;
            }

            set
            {
                _LocalPort = value;
            }
        }

        public ESPWifiData(string IP, int Port)
        {
            _IPAdresse = IP;
            _LocalPort = Port;
        }

        public override string ToString()
        {
            return _IPAdresse + ": " + LocalPort;
        }

        public static implicit operator string(ESPWifiData d)
        {
            return d.IPAdresse + ": " + d.LocalPort;
        }
    }

    public class NotificationBase : INotifyPropertyChanged
    {
        public event PropertyChangedEventHandler PropertyChanged;

        // SetField (Name, value); // where there is a data member
        protected bool SetProperty<T>(ref T field, T value, [CallerMemberName] String property
           = null)
        {
            if (EqualityComparer<T>.Default.Equals(field, value)) return false;
            field = value;
            RaisePropertyChanged(property);
            return true;
        }

        // SetField(()=> somewhere.Name = value; somewhere.Name, value) 
        // Advanced case where you rely on another property
        protected bool SetProperty<T>(T currentValue, T newValue, Action DoSet,
            [CallerMemberName] String property = null)
        {
            if (EqualityComparer<T>.Default.Equals(currentValue, newValue)) return false;
            DoSet.Invoke();
            RaisePropertyChanged(property);
            return true;
        }

        protected void RaisePropertyChanged(string property)
        {
            if (PropertyChanged != null)
            {
                PropertyChanged(this, new PropertyChangedEventArgs(property));
            }
        }
    }

    public class NotificationBase<T> : NotificationBase where T : class, new()
    {
        protected T This;

        public static implicit operator T(NotificationBase<T> thing) { return thing.This; }

        public NotificationBase(T thing = null)
        {
            This = (thing == null) ? new T() : thing;
        }
    }
}
