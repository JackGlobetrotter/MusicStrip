using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
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
            this.InitializeComponent();
            IPResults.Items.Add("4567898765RDFGHJI");
            this.UpdateLayout();
   
            //    StartScan();
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
                          if (await TryHost(SubNet + ip, 23, 1000))
                          {
                              ESPResults.Add(SubNet + ip + ":");
                              IPResults.Items.Add(SubNet + ip + ":");                             
                              this.UpdateLayout();
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
                System.Diagnostics.Debug.WriteLine(IP+ Port.ToString());
                cts.CancelAfter(TimeOut);
                await clientSocket.ConnectAsync(new HostName(IP), Port.ToString()).AsTask(cts.Token);
                await clientSocket.CancelIOAsync();
                clientSocket.Dispose();
                return true;

            }
            catch (TaskCanceledException)
            {
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
        public void RefreshView()
        {
            var oldItems = _view.ToArray();

            var tasks = _store.Where<TaskItem>(task => IncludeCompletedItems || !task.Completed);

            _view.Clear();
            _view.AddRange(tasks);
            if (CollectionChanged != null)
            {
                // Call the event handler for the updated list.
                CollectionChanged(this, new NotifyCollectionChangedEventArgs(NotifyCollectionChangedAction.Reset));
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
