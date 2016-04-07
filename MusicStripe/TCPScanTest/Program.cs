using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Net.Sockets;
using System.Net;
using System.Net.NetworkInformation;

namespace TCPScanTest
{
    class Program
    {
        static void Main(string[] args)
        {
            List<NetworkInterface> locals = new List<NetworkInterface>();
           
            foreach (NetworkInterface item in NetworkInterface.GetAllNetworkInterfaces())
            {
                if (item.OperationalStatus == OperationalStatus.Up) //&& item.NetworkInterfaceType == ?
                {
                    locals.Add(item);
                    Console.WriteLine(item.Name);
                }
               
            }


            string[] tester = new string[4];
            foreach (UnicastIPAddressInformation ip in locals[System.Convert.ToInt32(Console.ReadLine())-1].GetIPProperties().UnicastAddresses)
            {
                //  ip.
               
                if (ip.Address.AddressFamily == AddressFamily.InterNetwork & !IPAddress.IsLoopback(ip.Address))
                {
                    Console.WriteLine(ip.Address.ToString());
                     tester = ip.Address.ToString().Split(new char[] { '.' });
                }
            }

            string subnetstring = tester[0] + "." + tester[1] + "." + tester[2] + ".";
            int parallel =8;
            for (int i = 0; i < parallel; i++)
            {
              //  Task.Factory.StartNew(() => {
                    for (int ip = (256/ parallel) *i ; ip < (256 / parallel) *( i+1); ip++)
                    {
                        Console.WriteLine(subnetstring + ip+":"+TryHost(subnetstring + ip, 23, 2000));
                    }
             //   });
            }
            Console.ReadKey();
        }

        static bool TryHost(string IP, int Port,int TimeOut)
        {
            using (var tcp = new TcpClient())
            {
                var ar = tcp.BeginConnect(IP, Port, null, null);
                using (ar.AsyncWaitHandle)
                {
                    //Wait 2 seconds for connection.
                    if (ar.AsyncWaitHandle.WaitOne(TimeOut, false))
                    {
                        try
                        {
                            tcp.EndConnect(ar);
                            return true;
                        }
                        catch
                        {
                            //EndConnect threw an exception.
                            //Most likely means the server refused the connection.
                        }
                    }
                    else
                    {
                        return false;
                    }
                }
                return false;
            }
            
        }
    }
}
