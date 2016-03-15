using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Navigation;

// The User Control item template is documented at http://go.microsoft.com/fwlink/?LinkId=234236

namespace WindowsControl
{
    public delegate void FrequenzyChangedHandel(object sender, FrequenzyEventArgs e);

    public delegate void ColorChangedHandel(object sender, ColorEventArgs e);
    public class ColorEventArgs : EventArgs
    {
        byte _RedValue;
        byte _GreenValue;
        byte _BlueValue;

        public byte RedValue
        {
            get
            {
                return _RedValue;
            }

           
        }

        public byte GreenValue
        {
            get
            {
                return _GreenValue;
            }

            
        }

        public byte BlueValue
        {
            get
            {
                return _BlueValue;
            }

            
        }

        public ColorEventArgs(byte Red, byte Green, byte Blue)
        {
            this._RedValue = Red;
            this._GreenValue = Green;
            this._BlueValue = Blue;
        } // eo ctor


    }
    public class FrequenzyEventArgs : EventArgs
    {
        byte _Frequenzy;

    

        public byte Frequenzy
        {
            get
            {
                return _Frequenzy;
            }

        
        }

        public FrequenzyEventArgs(byte Freq)
        {
            this._Frequenzy = Freq;
        } // eo ctor


    }
    public sealed partial class LEDColorControl : UserControl
    {
        string _LEDName;
        byte count = 0;

        public string LEDNameString
        {
            get
            {
                return _LEDName;
            }

            set
            {
                _LEDName = value;
            }
        }
        public event FrequenzyChangedHandel FrequenzyChanged;
        public event ColorChangedHandel ColorChanged;
        public LEDColorControl()
        {
            LEDNameString = "test";
            this.DataContext = this;

            this.InitializeComponent();
            LEDName.Text = LEDNameString;
           // VisualStateManager.GoToState(this, LEDMixedState.Name, true);
        }

        private void LED_R_ValueChanged(object sender, RangeBaseValueChangedEventArgs e)
        {
            if (LED_R != null)
                if (LEDStateToggle.IsOn && count == 0)
                {
                    ColorChanged(this, new ColorEventArgs((byte)LED_R.Value, (byte)LED_G.Value, (byte)LED_B.Value));
                    count++;
                }
                else if (LEDStateToggle.IsOn && count < 3)
                    count++;
                else if (LEDStateToggle.IsOn && count > 2)
                    count = 0;
                else if (!LEDStateToggle.IsOn)
                    ColorChanged(this, new ColorEventArgs((byte)LED_R.Value, (byte)LED_G.Value, (byte)LED_B.Value));
        }

        private void LED_G_ValueChanged(object sender, RangeBaseValueChangedEventArgs e)
        {
            if (LED_R != null)
                if (LEDStateToggle.IsOn && count == 0)
                {
                    ColorChanged(this, new ColorEventArgs((byte)LED_R.Value, (byte)LED_G.Value, (byte)LED_B.Value));
                    count++;
                }
                else if (LEDStateToggle.IsOn && count < 3)
                    count++;
                else if (LEDStateToggle.IsOn && count > 2)
                    count = 0;
                else if (!LEDStateToggle.IsOn)
                    ColorChanged(this, new ColorEventArgs((byte)LED_R.Value, (byte)LED_G.Value, (byte)LED_B.Value));
        }

        private void LED_B_ValueChanged(object sender, RangeBaseValueChangedEventArgs e)
        {
            if (LED_R != null)
                if (LEDStateToggle.IsOn && count == 0)
                {
                    ColorChanged(this, new ColorEventArgs((byte)LED_R.Value, (byte)LED_G.Value, (byte)LED_B.Value));
                    count++;
                }
                else if (LEDStateToggle.IsOn && count < 2)
                    count++;
                else if (LEDStateToggle.IsOn && count >= 2)
                    count = 0;
                else if (!LEDStateToggle.IsOn)
                    ColorChanged(this, new ColorEventArgs((byte)LED_R.Value, (byte)LED_G.Value, (byte)LED_B.Value));
        }

        private void LED_Freq_ValueChanged(object sender, RangeBaseValueChangedEventArgs e)
        {
            FrequenzyChanged(this, new FrequenzyEventArgs((byte)LED_Freq.Value));
        }
    }
}
