using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.Storage.Streams;
using Windows.UI;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Media.Imaging;
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
        public ColorEventArgs(byte[] RGB)
        {
            this._RedValue = RGB[0];
            this._GreenValue = RGB[1];
            this._BlueValue = RGB[2];
        }

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
        } 


    }
    public sealed partial class LEDColorControl : UserControl
    {

        public WriteableBitmap tt {
            get; set; }
        bool setting = false;
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
         public  LEDColorControl()
        {


            tt = new WriteableBitmap(720, 510);


            int count = 0;
            //  tt = new WriteableBitmap(360, 255);
            byte[] col = new byte[4 * 720 * 255 * 2];

            for (int x = 0; x < 255; x++)
            {
                double brightness = x;
                for (int y = 0; y < 720; y++)
                {


                    ColorUtil.HsvToRgb(y/2, (double)x / (double)254, 1, out col[count], out col[count + 1], out col[count + 2], out col[count + 3]);
                    //   tt.SetPixel(x, y,  col[0],  col[1],  col[2]);
                    count = count + 4;

                }
            }

            for (int x = 254; x >= 0; x--)
            {
                // double brightness = x;
                for (int y = 0; y < 720; y++)
                {


                    ColorUtil.HsvToRgb(y/2, 1, (double)x / (double)254, out col[count], out col[count + 1], out col[count + 2], out col[count + 3]);
                    //   tt.SetPixel(x, y,  col[0],  col[1],  col[2]);
                    count = count + 4;

                }
            }
            //  tt.FromByteArray(HUEColors.HUE);
           

           
            using (Stream stream = tt.PixelBuffer.AsStream())
            {
                 stream.WriteAsync(col, 0, col.Length);
            }
          

            //tt.ForEach((x, y, color) => Color.FromArgb(color.A, (byte)(color.R / 2), (byte)(x * y), 100));
            //  tt.SetPixel(0, 0, 0, 0, 0, 0);
            LEDNameString = "test";
            this.DataContext = this;
          //  HSB_UI = new Image();
 
           
            this.InitializeComponent();
            HSB_UI.Source = tt;

            LEDName.Text = LEDNameString;
          //  BinaryWriter wr = new BinaryWriter(File.Create("text.txt"));
         
        // VisualStateManager.GoToState(this, LEDMixedState.Name, true);
    }


        public void SetData(byte[] DATA)
        {
           
            setting = true;
        
            LED_Freq.Value = DATA[0];
            LED_R.Value = DATA[1];
            LED_G.Value = DATA[2];

            LED_B.Value = DATA[3];
            setting = false;
        }

  

        private void LED_R_ValueChanged(object sender, RangeBaseValueChangedEventArgs e)
        {
            if (LED_R != null && !setting)
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
            if (LED_R != null&&!setting )
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
            if (LED_R != null && !setting)
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
            if(FrequenzyChanged!=null && !setting)
                FrequenzyChanged(this, new FrequenzyEventArgs((byte)LED_Freq.Value));
        }
        bool SavePosition;
        byte[] pos = new byte[2];
        private void HSB_UI_PointerPressed(object sender, PointerRoutedEventArgs e)
        {
            pos[0] = (byte)(((e.GetCurrentPoint(HSB_UI) as Windows.UI.Input.PointerPoint).Position.X) / 2);
            pos[1] = (byte)(((e.GetCurrentPoint(HSB_UI) as Windows.UI.Input.PointerPoint).Position.Y) / 2);
            SavePosition = true;

        }

        private void HSB_UI_PointerReleased(object sender, PointerRoutedEventArgs e)
        {
            ColorChanged(this, new ColorEventArgs((byte)LED_R.Value, (byte)LED_G.Value, (byte)LED_B.Value)); SavePosition = false;
        }

        private void HSB_UI_PointerExited(object sender, PointerRoutedEventArgs e)
        {
            if (SavePosition)
            {
                if (pos[0] <= 20)
                    pos[0] = 0;
                else if (pos[0] >= 240)
                    pos[0] = 255;

                if (pos[1] <= 20)
                    pos[1] = 0;
                else if (pos[1] >= 240)
                    pos[1] = 255;
                ColorChanged(this, new ColorEventArgs(HsvToRgb(pos[0],255,pos[1])));
                SavePosition = false;
            }
        }
        class MemoryRandomAccessStream : IRandomAccessStream
        {
            private Stream m_InternalStream;

            public MemoryRandomAccessStream(Stream stream)
            {
                this.m_InternalStream = stream;
            }

            public MemoryRandomAccessStream(byte[] bytes)
            {
                this.m_InternalStream = new MemoryStream(bytes);
            }

            public IInputStream GetInputStreamAt(ulong position)
            {
                this.m_InternalStream.Seek((long)position, SeekOrigin.Begin);

                return this.m_InternalStream.AsInputStream();
            }

            public IOutputStream GetOutputStreamAt(ulong position)
            {
                this.m_InternalStream.Seek((long)position, SeekOrigin.Begin);

                return this.m_InternalStream.AsOutputStream();
            }

            public ulong Size
            {
                get { return (ulong)this.m_InternalStream.Length; }
                set { this.m_InternalStream.SetLength((long)value); }
            }

            public bool CanRead
            {
                get { return true; }
            }

            public bool CanWrite
            {
                get { return true; }
            }

            public IRandomAccessStream CloneStream()
            {
                throw new NotSupportedException();
            }

            public ulong Position
            {
                get { return (ulong)this.m_InternalStream.Position; }
            }

            public void Seek(ulong position)
            {
                this.m_InternalStream.Seek((long)position, 0);
            }

            public void Dispose()
            {
                this.m_InternalStream.Dispose();
            }

            public Windows.Foundation.IAsyncOperationWithProgress<IBuffer, uint> ReadAsync(IBuffer buffer, uint count, InputStreamOptions options)
            {
                var inputStream = this.GetInputStreamAt(0);
                return inputStream.ReadAsync(buffer, count, options);
            }

            public Windows.Foundation.IAsyncOperation<bool> FlushAsync()
            {
                var outputStream = this.GetOutputStreamAt(0);
                return outputStream.FlushAsync();
            }

            public Windows.Foundation.IAsyncOperationWithProgress<uint, uint> WriteAsync(IBuffer buffer)
            {
                var outputStream = this.GetOutputStreamAt(0);
                return outputStream.WriteAsync(buffer);
            }
        }
        byte[] HsvToRgb(double h, double S, double V)
        {
            double H = h;
            while (H < 0) { H += 360; };
            while (H >= 360) { H -= 360; };
            double R, G, B;
            if (V <= 0)
            { R = G = B = 0; }
            else if (S <= 0)
            {
                R = G = B = V;
            }
            else
            {
                double hf = H / 60.0;
                int i = (int)Math.Floor(hf);
                double f = hf - i;
                double pv = V * (1 - S);
                double qv = V * (1 - S * f);
                double tv = V * (1 - S * (1 - f));
                switch (i)
                {

                    // Red is the dominant color

                    case 0:
                        R = V;
                        G = tv;
                        B = pv;
                        break;

                    // Green is the dominant color

                    case 1:
                        R = qv;
                        G = V;
                        B = pv;
                        break;
                    case 2:
                        R = pv;
                        G = V;
                        B = tv;
                        break;

                    // Blue is the dominant color

                    case 3:
                        R = pv;
                        G = qv;
                        B = V;
                        break;
                    case 4:
                        R = tv;
                        G = pv;
                        B = V;
                        break;

                    // Red is the dominant color

                    case 5:
                        R = V;
                        G = pv;
                        B = qv;
                        break;

                    // Just in case we overshoot on our math by a little, we put these here. Since its a switch it won't slow us down at all to put these here.

                    case 6:
                        R = V;
                        G = tv;
                        B = pv;
                        break;
                    case -1:
                        R = V;
                        G = pv;
                        B = qv;
                        break;

                    // The color is not defined, we should throw an error.

                    default:
                        //LFATAL("i Value error in Pixel conversion, Value is %d", i);
                        R = G = B = V; // Just pretend its black/white
                        break;
                }
            }
            return new byte[] { Clamp((byte)(R * 255.0)), Clamp((byte)(G * 255.0)), Clamp((byte)(B * 255.0)) };

        }

        /// <summary>
        /// Clamp a value to 0-255
        /// </summary>
        byte Clamp(byte i)
        {
            if (i < 0) return 0;
            if (i > 255) return 255;
            return i;
        }

        private void HSB_UI_PointerMoved(object sender, PointerRoutedEventArgs e)
        {
            if(SavePosition)
            { pos[0] = (byte)(((e.GetCurrentPoint(HSB_UI) as Windows.UI.Input.PointerPoint).Position.X)/2);
                pos[1] = (byte)(((e.GetCurrentPoint(HSB_UI) as Windows.UI.Input.PointerPoint).Position.Y) / 2);
            }
        }
    }

    public static class ColorUtil
    {
        /// <summary>
        /// Convert HSV to RGB
        /// h is from 0-360
        /// s,v values are 0-1
        /// r,g,b values are 0-255
        /// Based upon http://ilab.usc.edu/wiki/index.php/HSV_And_H2SV_Color_Space#HSV_Transformation_C_.2F_C.2B.2B_Code_2
        /// </summary>
        public static void HsvToRgb(double h, double S, double V, out byte b, out byte g, out byte r,out byte a)
        {
            // ######################################################################
            // T. Nathan Mundhenk
            // mundhenk@usc.edu
            // C/C++ Macro HSV to RGB
            a = 255;
            double H = h;
            while (H < 0) { H += 360; };
            while (H >= 360) { H -= 360; };
            double R, G, B;
            if (V <= 0)
            { R = G = B = 0; }
            else if (S <= 0)
            {
                R = G = B = V;
            }
            else
            {
                double hf = H / 60.0;
                int i = (int)Math.Floor(hf);
                double f = hf - i;
                double pv = V * (1 - S);
                double qv = V * (1 - S * f);
                double tv = V * (1 - S * (1 - f));
                switch (i)
                {

                    // Red is the dominant color

                    case 0:
                        R = V;
                        G = tv;
                        B = pv;
                        break;

                    // Green is the dominant color

                    case 1:
                        R = qv;
                        G = V;
                        B = pv;
                        break;
                    case 2:
                        R = pv;
                        G = V;
                        B = tv;
                        break;

                    // Blue is the dominant color

                    case 3:
                        R = pv;
                        G = qv;
                        B = V;
                        break;
                    case 4:
                        R = tv;
                        G = pv;
                        B = V;
                        break;

                    // Red is the dominant color

                    case 5:
                        R = V;
                        G = pv;
                        B = qv;
                        break;

                    // Just in case we overshoot on our math by a little, we put these here. Since its a switch it won't slow us down at all to put these here.

                    case 6:
                        R = V;
                        G = tv;
                        B = pv;
                        break;
                    case -1:
                        R = V;
                        G = pv;
                        B = qv;
                        break;

                    // The color is not defined, we should throw an error.

                    default:
                        //LFATAL("i Value error in Pixel conversion, Value is %d", i);
                        R = G = B = V; // Just pretend its black/white
                        break;
                }
            }
            r = Clamp((byte)(R * 255.0));
            g = Clamp((byte)(G * 255.0));
            b = Clamp((byte)(B * 255.0));
        }

        /// <summary>
        /// Clamp a value to 0-255
        /// </summary>
        private static byte Clamp(byte i)
        {
            if (i < 0) return 0;
            if (i > 255) return 255;
            return i;
        }
    }
}
