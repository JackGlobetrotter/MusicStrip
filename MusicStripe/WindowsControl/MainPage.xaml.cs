using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using System.Threading.Tasks;
using Windows.Devices.Enumeration;
using Windows.Devices.SerialCommunication;
using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.Networking.Sockets;
using Windows.Storage.Streams;
using Windows.UI;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Navigation;
using System.Net;
using Windows.Networking;
using Windows.ApplicationModel.Core;
using Windows.UI.Core;
using Windows.UI.ViewManagement;
using Windows.UI.Popups;
// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=402352&clcid=0x409

namespace WindowsControl
{
    public class BooleanToVisibilityConverter : IValueConverter
    {
        /// <summary>
        /// Converts a bool value to a Visibility value.
        /// </summary>
        /// <returns>
        /// Returns Visibility.Visible if true, else Visibility.Collapsed.
        /// </returns>
        public object Convert(object value, Type targetType, object parameter, string language)
        {
            var b = value as bool?;
            return b == null ? Visibility.Collapsed : (b.Value ? Visibility.Visible : Visibility.Collapsed);
        }

        /// <summary>
        /// Converts a Visibility value to a bool value.
        /// </summary>
        /// <returns>
        /// Returns true if Visiblility.Visible, else false.
        /// </returns>
        public object ConvertBack(object value, Type targetType, object parameter, string language)
        {
            var v = value as Visibility?;
            return v == null ? (object)null : v.Value == Visibility.Visible;
        }
    }

    public class RGBToBrush : IValueConverter
    { 
        public object Convert(object value, Type targetType, object parameter, string language)
        {
          //  return new SolidColorBrush(Colors.Red);
            if (parameter == null)
            {
                if (value != null)
                {
                    byte[] bytes = BitConverter.GetBytes(System.Convert.ToInt32(value));
                   // return new SolidColorBrush(Colors.Red);
                    return new SolidColorBrush(Color.FromArgb(255, bytes[0], bytes[1], bytes[2]));
                }
                else return new SolidColorBrush(Colors.Red);
                return new SolidColorBrush(Colors.Black);
            }
            else
            {
                if (value != null)
                {
                    switch (System.Convert.ToInt32(parameter)) {
                        case 1:
                            return new SolidColorBrush(Color.FromArgb(System.Convert.ToByte(value), System.Convert.ToByte(value), 0, 0));
                            break;
                        case 2:
                            return new SolidColorBrush(Color.FromArgb(System.Convert.ToByte(value),0, System.Convert.ToByte(value), 0));
                            break;
                        case 3:
                            return new SolidColorBrush(Color.FromArgb(System.Convert.ToByte(value),0,0, System.Convert.ToByte(value)));
                            break;

                    } }
                else return new SolidColorBrush(Colors.Red);


                
            }
           return new SolidColorBrush(Colors.Black);
        }

   

        public object ConvertBack(object value, Type targetType, object parameter, string language)
        {
            if (value != null)
                return new byte[] { (value as Color?).Value.R, (value as Color?).Value.G, (value as Color?).Value.B };
            else
                return new byte[] { 0, 0, 0 };
        }

    }


    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class MainPage : Page
    {
        Windows.Storage.ApplicationDataContainer localSettings = Windows.Storage.ApplicationData.Current.LocalSettings;
        Windows.Storage.StorageFolder localFolder = Windows.Storage.ApplicationData.Current.LocalFolder;
        bool WifiDataChanged=false;
        StreamSocket ESP;
        IPAddress _ESP_IP;
        string _SSID;
        string _PWD;
        DataWriter ESP_Stream;
        byte ESP_Port { get; set; }
        bool loading = true;
        public SolidColorBrush Led1 { get; set; }
        private byte[] _LED1;
        private bool _LightState;
        bool LEDDetailed = false;
        bool firstchange = true;
        public  MainPage()
        {
            
            ESP_Port = 80;
            ESP_IP = IPAddress.Parse("192.168.4.1");
            Led1 = new SolidColorBrush(Colors.CadetBlue);
            Windows.Storage.ApplicationDataCompositeValue WifiConfig =
          (Windows.Storage.ApplicationDataCompositeValue)localSettings.Values["WifiConfig"];

            if (WifiConfig == null || WifiConfig.Count != 4)
            {
                // No data
            }
            else
            {
                ESP_Port = System.Convert.ToByte(WifiConfig["Port"]);
                SSID = (String)WifiConfig["SSID"];
                PWD = (String)WifiConfig["PWD"];
                ESP_IP = IPAddress.Parse((String)WifiConfig["IP"]);

                // Access data in composite["intVal"] and composite["strVal"]
            }
            this.DataContext = this;
            LED1 = new byte[] { 255, 255, 0 };
            this.InitializeComponent();
            VisualStateManager.GoToState(this, LEDMixedState.Name, true);
            LightState = true;

            LED1State.Items.Add(ControlByte.FadeState.ToString());
            LED1State.Items.Add(ControlByte.FixColorState.ToString());
            LED1State.Items.Add(ControlByte.MusicState.ToString());
            LED1State.Items.Add(ControlByte.SmoothState.ToString());
            LED1State.Items.Add(ControlByte.FlashState.ToString());
            LED1State.SelectedIndex = 0;

            LED2State.Items.Add(ControlByte.FadeState.ToString());
            LED2State.Items.Add(ControlByte.FixColorState.ToString());
            LED2State.Items.Add(ControlByte.MusicState.ToString());
            LED2State.Items.Add(ControlByte.SmoothState.ToString());
            LED2State.Items.Add(ControlByte.FlashState.ToString());
            LED2State.SelectedIndex = 0;

            loading = false;

            // Composite setting

       
        }

       

        public bool LightState
        {
            get
            {
                return _LightState;
            }

            set
            {
                _LightState = value;
            }
        }

        public byte[] LED1
        {
            get
            {
                return _LED1;
            }

            set
            {
                _LED1 = value;
            }
        }

        public IPAddress ESP_IP
        {
            get
            {
                return _ESP_IP;
            }

            set
            {
                _ESP_IP = value;
            }
        }

        public string SSID
        {
            get
            {
                return _SSID;
            }

            set
            {
                _SSID = value;
            }
        }

        public string PWD
        {
            get
            {
                return _PWD;
            }

            set
            {
                _PWD = value;
            }
        }

        private void LightSwitch_Toggled(object sender, RoutedEventArgs e)
        {
            byte test = System.Convert.ToByte(LightState);
            if (ESP != null)
           
            ArduinoSend(ControlByte.LightToggle, new byte[] { System.Convert.ToByte(LightState) });
        }

        private void LEDState_Toggled(object sender, RoutedEventArgs e)
        {
            LEDDetailed = !LEDDetailed;
            if (LEDDetailed)
                VisualStateManager.GoToState(this, LEDDetailState.Name, true);
            else
                VisualStateManager.GoToState(this, LEDMixedState.Name, true);
        
        }

        private void LED1Slider_ValueChanged(object sender, RangeBaseValueChangedEventArgs e)
        {

            if (!loading & ESP != null)
            {
                LED1Color.SelectedColor = new SolidColorBrush(Color.FromArgb(255, (byte)LED1R.Value, (byte)LED1G.Value, (byte)LED1B.Value));
                LED1Color.SelectedColor = new SolidColorBrush(Color.FromArgb(255, (byte)LED1R.Value, (byte)LED1G.Value, (byte)LED1B.Value));

                //    LED1Color_SelectedColorChanged(sender, new EventArgs());
            }
        }



        private async void ArduinoSend(ControlByte Controler, byte[] Data)
        {
            if (ESP_Stream == null)
                return;
            ESP_Stream.WriteByte((byte)Controler);
            ESP_Stream.WriteBytes(Data);
            ESP_Stream.WriteByte((byte)ControlByte.Stop);
            await ESP_Stream.StoreAsync();
            await ESP_Stream.FlushAsync();

        }

        enum ControlByte : byte
        {
            //WIFI
            Connect = 1,
            Reconnect,
            Port,
            SSID,
            PWD,
            IP,
            WifiConnectSate,
            //ESP8266
            OTAUpdate,
            OTAStart,
            OTAEnd,
            OTAError,
            //General
            Start,
            Stop,
            GetLightState,
            RGBColor,
            MusicTitle,
            //LED
            LED1SwitchStade,
            LED2SwitchStade,
            LED1Data,
            LED2Data,
            LEDState,

            LightToggle,
            MusicState,
            MicroState,
            FadeState,
            SmoothState,
            FlashState,
            FixColorState,

            LED1Frequency,
            LED2Frequency
        };

        private void LED1State_SelectionChanged(object sender, SelectionChangedEventArgs e)

        {
            ArduinoSend(ControlByte.LED1SwitchStade, new byte[] { (byte)Enum.Parse(typeof(ControlByte), (sender as ComboBox).SelectedItem.ToString(), true) });
            
        }

        private async void Connect_Click(object sender, RoutedEventArgs e)
        {
            StreamSocketListener listener = new StreamSocketListener();
        
            // connect ESP, read Lightstat
            ESP = new StreamSocket();
            HostName hostName
                 = new HostName( IPBox.Text.ToString());
        


            // If necessary, tweak the socket's control options before carrying out the connect operation.
            // Refer to the StreamSocketControl class' MSDN documentation for the full list of control options.
            ESP.Control.KeepAlive = false;

            // Save the socket, so subsequent steps can use it.
  
                    await ESP.ConnectAsync(hostName,PortBox.Text);

            ESP_Stream = new DataWriter( ESP.OutputStream);
            ConnectionState.Fill = new SolidColorBrush(Colors.Green);
        }

        private void LED1OnOff_Toggled(object sender, RoutedEventArgs e)
        {
            if((sender as ToggleSwitch).Name.Contains("1"))
            ArduinoSend(ControlByte.LEDState, new byte[] { 0, System.Convert.ToByte((sender as ToggleSwitch).IsOn) });
            else
                ArduinoSend(ControlByte.LEDState, new byte[] { 1, System.Convert.ToByte((sender as ToggleSwitch).IsOn) });
        }

        private void LED2State_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {

        }

        private void LED1Color_PointerReleased(object sender, PointerRoutedEventArgs e)
        {
      //      ArduinoSend(ControlByte.LED1Data, new byte[] { (byte)(sender as ColorPicker.ColorPicker).SelectedColor.Color.R, (byte)(sender as ColorPicker.ColorPicker).SelectedColor.Color.G, (byte)(sender as ColorPicker.ColorPicker).SelectedColor.Color.B });

        }

        private void LED1Color_SelectedColorChanged(object sender, EventArgs e)
        {
            if (!firstchange)
            {
                ArduinoSend(ControlByte.LED1Data, new byte[] { (byte)LED1Color.SelectedColor.Color.R, (byte)LED1Color.SelectedColor.Color.G, (byte)LED1Color.SelectedColor.Color.B });
                firstchange = !firstchange;
            }
            else
                firstchange = !firstchange;

        }

   
        static byte[] GetBytes(string str)
        {
            byte[] bytes = new byte[str.Length+1];
            bytes[0] =(byte) str.Length;
            for (int i = 1; i <= str.Length; i++)
            {
                bytes[i]= (byte)str[i-1];
            }

            return bytes;
        }

        static string GetString(byte[] bytes)
        {
            char[] chars = new char[bytes.Length / sizeof(char)];
            System.Buffer.BlockCopy(bytes, 0, chars, 0, bytes.Length);
            return new string(chars);
        }
        private async void Disconnect_Click(object sender, RoutedEventArgs e)
        {

            ESP_Stream.WriteByte((byte)ControlByte.Reconnect);

            await ESP_Stream.StoreAsync();
            await ESP_Stream.FlushAsync();
        }


        //private async void WriteWifiData_Click(object sender, RoutedEventArgs e)
        //{

        //    ArduinoSend(ControlByte.SSID, GetBytes(SSIDBox.Text));
        //   await System.Threading.Tasks.Task.Delay(300);
        //    ArduinoSend(ControlByte.Port, new byte[] { (byte)PortSlider.Value });
        //    await System.Threading.Tasks.Task.Delay(300);
        //    ArduinoSend(ControlByte.PWD, GetBytes(PWDBox.Text));

        //    Windows.Storage.ApplicationDataCompositeValue composite = new Windows.Storage.ApplicationDataCompositeValue();
        //    composite["Port"] = PortSlider.Value;
        //    composite["SSID"] = SSIDBox.Text;
        //    composite["PWD"] = PWDBox.Text;
        //    composite["IP"] = IPBox.Text;
        //    localSettings.Values["WifiConfig"] = composite;
        //}

   


        private async void Button_Click(object sender, RoutedEventArgs e)
        {
            var btn = sender as Button;
            var result = await SettingsDialog.ShowAsync();
            btn.Content = "Result: " + result;
            if (WifiDataChanged)
            {
                var dialog = new MessageDialog("Changed on Wifi Data will take effect on reboot. \r\nDo you want to reboot Now?","Reboot?");
                dialog.Commands.Add(new UICommand { Label = "Yes", Id = 0 });
                dialog.Commands.Add(new UICommand { Label = "No", Id = 1 });
                var res = await dialog.ShowAsync();

                if ((int)res.Id == 0)
                { ArduinoSend(ControlByte.Reconnect, new byte[0]); }
            }
        }

        private void SettingsSelect_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {

        }

        private async void WifiSettingsPage_OTAModeSet(object sender, EventArgs e)
        {
            ESP_Stream.WriteByte((byte)ControlByte.OTAUpdate);

            await ESP_Stream.StoreAsync();
            await ESP_Stream.FlushAsync();
        }

        private void WifiSettingsPage_WifiDataSet(object sender, WifiEventArgs e)
        {
            WifiDataChanged = true;
        }
    }

}
