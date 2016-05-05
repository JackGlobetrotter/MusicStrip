using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.Networking;
using Windows.Networking.Sockets;
using Windows.Storage.Streams;
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
    public sealed partial class ESPDataUI : UserControl
    {
        StreamSocket ESP;
        public ESPData Data {
            get; set; }
        public  ESPDataUI(ESPWifiData WifiData)
        {
          
            this.InitializeComponent();
     
            this.DataContext = Data;
             Connect(WifiData);
        }
        public async void Connect(ESPWifiData WifiData)
        {
            ESP = new StreamSocket();
            HostName hostName
              = new HostName(WifiData.IPAdresse);



            // If necessary, tweak the socket's control options before carrying out the connect operation.
            // Refer to the StreamSocketControl class' MSDN documentation for the full list of control options.
            ESP.Control.KeepAlive = false;

            // Save the socket, so subsequent steps can use it.

            await ESP.ConnectAsync(hostName, WifiData.LocalPort.ToString());

            var ESP_Stream = new DataWriter(ESP.OutputStream);
            var dr = new DataReader(ESP.InputStream);
            dr.InputStreamOptions = InputStreamOptions.None;

            ESP_Stream.WriteByte((byte)ControlByte.GetData);
            ESP_Stream.StoreAsync();//send type|LCD|LEDcount|each[RedPin,GreenPin,BluePin,active,Red,Green,Blue,frequenzy]
            byte[] Basicdata = new byte[3];

            for (int i = 0; i < 3; i++)
            {
                await dr.LoadAsync(1);
                Basicdata[i] = dr.ReadByte();
            }
            byte[] LEDData = new byte[Basicdata[2]*9];
            LEDHard[] LEDS = new LEDHard[Basicdata[2]];

            for (int i = 0; i < Basicdata[2]*9; i++)
            {
                await dr.LoadAsync(1);
                LEDData[i] = dr.ReadByte();
            }
            for (int i = 0; i < Basicdata[2]; i++)
            {
                LEDS[i]=new LEDHard(LEDData.Skip(i).Take(9).ToArray());
            }
            Data = new ESPData(WifiData,System.Convert.ToBoolean(Basicdata[1]),LEDS);
            await this.Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.High, () =>
            {
                foreach (var item in Data.LEDs)
                {
                    LEDColorControl LedControl = new LEDColorControl();
                    LedControl.SetData(new byte[] { item.Frequenzy, item.Colors[0], item.Colors[2], item.Colors[2] });
                    MainControl.Children.Add(LedControl);//create proper led view
                }
             


            });
          this.DataContext = Data;

        }
    }
}
