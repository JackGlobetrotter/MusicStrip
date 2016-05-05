using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Collections.Specialized;
using System.ComponentModel;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Net.NetworkInformation;
using System.Runtime.InteropServices.WindowsRuntime;
using System.Threading;
using System.Threading.Tasks;
using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.Networking;
using Windows.Networking.Connectivity;
using Windows.Networking.Sockets;
using Windows.Storage.Streams;
using Windows.UI.Core;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Navigation;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

namespace WindowsControl
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class ScanNetwork : Page
    {
        private string _LocalIP;
        private string _StandartIP;
        private string _SubNet;
        private ObservableCollection<ESPWifiData> _ESPResults;
        public delegate void ESPSelectedHandel(object sender, WifiAPEventArgs e);
        public event ESPSelectedHandel ESPSelected;

        public  ScanNetwork()
        {
            _ESPResults = new ObservableCollection<ESPWifiData>();
            this.DataContext = this;
            var icp = NetworkInformation.GetInternetConnectionProfile();
            if (icp != null && icp.NetworkAdapter != null && icp.NetworkAdapter.NetworkAdapterId != null)
            {
                var name = icp.ProfileName;

                var hostnames = NetworkInformation.GetHostNames();
  
                foreach (var hn in hostnames)
                {
                   
                
                    if (hn.IPInformation != null &&
                        hn.IPInformation.NetworkAdapter != null &&
                        hn.IPInformation.NetworkAdapter.NetworkAdapterId != null &&
                        hn.IPInformation.NetworkAdapter.NetworkAdapterId == icp.NetworkAdapter.NetworkAdapterId &&
                        hn.Type == HostNameType.Ipv4)
                    {
                        LocalIP = hn.CanonicalName;
              
                        SubNet = LocalIP.Split('.')[0] + "." + LocalIP.Split('.')[1] + "." + LocalIP.Split('.')[2];
                    }
                }
            }
            NotifyPropertyChanged("ESPResults");
            //  NotifyPropertyChanged("lastItem");
            ESPResults.CollectionChanged += ESPResults_CollectionChanged;
            //ESPResults.Add("sdfghdgf");
            this.InitializeComponent();
            // ESPResults.Add("sdfghjqsserdtfyghujklds");

            /*  StartScan(23);
          StartScan(80);*/
            _ESPResults.Add(new ESPWifiData("192.168.1.82", 80));

        }

        private void ESPResults_CollectionChanged(object sender, System.Collections.Specialized.NotifyCollectionChangedEventArgs e)
        {
            ObservableCollection<ESPWifiData> obsSender = sender as ObservableCollection<ESPWifiData>;

            List<ESPWifiData> editedOrRemovedItems = new List<ESPWifiData>();

            if (e.NewItems != null)
            {
                foreach (ESPWifiData newItem in e.NewItems)
                {
                    editedOrRemovedItems.Add(newItem);
                }
            }

            if (e.OldItems != null)
            {
                foreach (ESPWifiData oldItem in e.OldItems)
                {
                    editedOrRemovedItems.Add(oldItem);
                }
            }

            // get the action which raised the collection changed event
            NotifyCollectionChangedAction action = e.Action;
        }

        public event PropertyChangedEventHandler PropertyChanged;
        private void NotifyPropertyChanged(string propertyName)
        {
            var handler = this.PropertyChanged;
            if (handler != null)
                handler(this, new PropertyChangedEventArgs(propertyName));
        }
        int _ResultCounter;
        public async 
        Task
StartScan(int Port)
        {
            int parallel = 8;
            for (int i = 0; i < parallel; i++)
            {
              //  ESPResults.Add("dsfghffdgrzerg0025");
            await     Task.Factory.StartNew(async (object state) =>
                  {
                      int run = (int)state;
                      for (int ip = (256 / parallel) * run; ip < (256 / parallel) * (run + 1); ip++)
                      {

                         // ESPResults.Add(ip + ":" + 23);
                          if (await TryHost(SubNet +"."+ ip, Port, 1000))
                          {
                              if(SubNet+ "." + ip != LocalIP)
                              await this.Dispatcher.TryRunAsync(CoreDispatcherPriority.Normal, (() => { ESPResults.Add(new ESPWifiData(SubNet + "." + ip  , Port)); } ));


                          }
                          if (ip == 255)
                              ResultCounter++;
                      }
                  
                  }, i);
            }


        }

        async Task<bool> TryHost(string IP, int Port, int TimeOut)
        {
            StreamSocket clientSocket = new StreamSocket();
            CancellationTokenSource cts = new CancellationTokenSource();

            try
            {

              //  System.Diagnostics.Debug.WriteLine(IP + ":" + Port.ToString());
                cts.CancelAfter(TimeOut);
                var task =  clientSocket.ConnectAsync(new HostName(IP), Port.ToString()).AsTask(cts.Token);
               // Debug.WriteLine(task.Id);
                await task;
              //  await clientSocket.CancelIOAsync();
                clientSocket.Dispose();
                return true;

            }
            catch (TaskCanceledException)
            {
                clientSocket.Dispose(); // Debug.WriteLine("Operation was cancelled.");
                return false;
               
            }
            catch (Exception ex) { return false; }
  
        }
        public string LocalIP
        {
            get
            {
                return _LocalIP;
            }

            set
            {
                _LocalIP = value;
            }
        }
      
        public ObservableCollection<ESPWifiData> ESPResults
        {
            get
            {
                return _ESPResults;
            }

            set
            {
                _ESPResults = value;
            }
        }

        public string SubNet
        {
            get
            {
                return _SubNet;
            }

            set
            {
                _SubNet = value;
            }
        }

        public string StandartIP
        {
            get
            {
                return _StandartIP;
            }

            set
            {
                _StandartIP = value;
            }
        }

        public int ResultCounter
        {
            get
            {
                return _ResultCounter;
            }

            set
            {
              //  Debug.WriteLine(value);
                if (value == 2)
                     this.Dispatcher.RunAsync(CoreDispatcherPriority.High, () =>
                    {
                        IPProgress.IsActive = false; IPProgressText.Text = "Finished";
                       
                      //  this.ScanFinished(this, new WifiAPEventArgs(_ESPResults.ToList<ESPWifiData>()));
                    });
                _ResultCounter = value;
            }
        }

        private async void IPResults_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            var ESP = new StreamSocket();
            HostName hostName
              = new HostName(_ESPResults[IPResults.SelectedIndex].IPAdresse);



            // If necessary, tweak the socket's control options before carrying out the connect operation.
            // Refer to the StreamSocketControl class' MSDN documentation for the full list of control options.
            ESP.Control.KeepAlive = false;

            // Save the socket, so subsequent steps can use it.

            await ESP.ConnectAsync(hostName, _ESPResults[IPResults.SelectedIndex].LocalPort.ToString());

            var ESP_Stream = new DataWriter(ESP.OutputStream);
            var dr = new DataReader(ESP.InputStream);
            dr.InputStreamOptions = InputStreamOptions.None;

            ESP_Stream.WriteByte((byte)ControlByte.Blink);
            ESP_Stream.WriteByte((byte)3);
            ESP_Stream.WriteByte((byte)ControlByte.Stop);
            await ESP_Stream.StoreAsync();
            ESP_Stream.Dispose();
            ESP.Dispose();
            
        }

        private void IPResults_DoubleTapped(object sender, DoubleTappedRoutedEventArgs e)
        {
            ESPSelected(this, new WifiAPEventArgs(ESPResults[(sender as ListBox).SelectedIndex]));
        }
    }
}
