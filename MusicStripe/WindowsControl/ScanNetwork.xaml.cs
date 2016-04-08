using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Collections.Specialized;
using System.ComponentModel;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using System.Threading;
using System.Threading.Tasks;
using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.Networking;
using Windows.Networking.Connectivity;
using Windows.Networking.Sockets;
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
        private ObservableCollection<string> _ESPResults;
        string SubNet = "";
        public ScanNetwork()
        {
            _ESPResults = new ObservableCollection<string>();
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
                        SubNet = LocalIP.Split('.')[0] + "." + LocalIP.Split('.')[1] + "." + LocalIP.Split('.')[2] + "." ;
                    }
                }
            }
            NotifyPropertyChanged("ESPResults");
            //  NotifyPropertyChanged("lastItem");
            ESPResults.CollectionChanged += ESPResults_CollectionChanged;
            //ESPResults.Add("sdfghdgf");
            this.InitializeComponent();
            ESPResults.Add("sdfghjqsserdtfyghujklds");

                StartScan();
        }

        private void ESPResults_CollectionChanged(object sender, System.Collections.Specialized.NotifyCollectionChangedEventArgs e)
        {
            ObservableCollection<string> obsSender = sender as ObservableCollection<string>;

            List<string> editedOrRemovedItems = new List<string>();

            if (e.NewItems != null)
            {
                foreach (string newItem in e.NewItems)
                {
                    editedOrRemovedItems.Add(newItem);
                }
            }

            if (e.OldItems != null)
            {
                foreach (string oldItem in e.OldItems)
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

        public void StartScan()
        {
            int parallel = 8;
            for (int i = 0; i < parallel; i++)
            {
                 Task.Factory.StartNew(async (object state) =>
                  {
                      int run = (int)state;
                      for (int ip = (256 / parallel) * run; ip < (256 / parallel) * (run + 1); ip++)
                      {
                   
                          if (await TryHost(SubNet + ip, 8080, 10))
                          {
                              ESPResults.Add(SubNet + ip + ":");
                            
                          }
                      }
                  },i);
            }

        }

        async Task<bool> TryHost(string IP, int Port, int TimeOut)
        {
            StreamSocket clientSocket = new StreamSocket();

     

            CancellationTokenSource cts = new CancellationTokenSource();

            try
            {
                ESPResults.Add(IP + ":"+ Port.ToString());
                System.Diagnostics.Debug.WriteLine(IP + ":" + Port.ToString());
                cts.CancelAfter(TimeOut);
                await clientSocket.ConnectAsync(new HostName(IP), Port.ToString()).AsTask(cts.Token);
              //  await clientSocket.CancelIOAsync();
                clientSocket.Dispose();
                return true;

            }
            catch (TaskCanceledException)
            {
                clientSocket.Dispose();
                return false;
                // Debug.WriteLine("Operation was cancelled.");
            }

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
      
        public ObservableCollection<string> ESPResults
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

       
    }
}
