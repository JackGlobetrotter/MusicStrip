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

// Pour plus d'informations sur le modèle d'élément Page vierge, voir la page http://go.microsoft.com/fwlink/?LinkId=234238

namespace WindowsControl
{
    public delegate void OTASetHandel(object sender, EventArgs e);
    public delegate void WifiDataHandel(object sender, WifiEventArgs e);

    /// <summary>
    /// Une page vide peut être utilisée seule ou constituer une page de destination au sein d'un frame.
    /// </summary>
    public sealed partial class WifiSettings : Page
    {
        public event OTASetHandel OTAModeSet;
        public event WifiDataHandel WifiDataSet;
        public WifiSettings()
        {
            this.InitializeComponent();
        }

        private void SetOtaMode_Click(object sender, RoutedEventArgs e)
        {
            OTAModeSet(this, EventArgs.Empty);
        }

        private void SendWifiData_Click(object sender, RoutedEventArgs e)
        {

            //var btn = sender as Button;
            //var result = 
            VisualStateManager.GoToState(this, WifiSetDataState.Name, true);
            //btn.Content = "Result: " + result;

        }

        private void SetData_Click(object sender, RoutedEventArgs e)
        {
            WifiDataSet(this, new WifiEventArgs(SSID.Text, Password.Password, (byte)PortSlider.Value));
        }

        private void CancelTransmission_Click(object sender, RoutedEventArgs e)
        {
            VisualStateManager.GoToState(this, SettingsViewState.Name, true);

        }
    }

    public class WifiEventArgs : EventArgs
    {
        private string _SSID;
        private string _PWD;
        private byte _Port;

        public string SSID
        {
            get
            {
                return _SSID;
            }

        
        }

        public string PWD
        {
            get
            {
                return _PWD;
            }


        }

        public byte Port
        {
            get
            {
                return _Port;
            }

        }

        public WifiEventArgs(string SSID,string Password, byte Port)
        {
            this._SSID = SSID;
            _PWD = Password;
            this._Port = Port;
        } // eo ctor


    }
}
