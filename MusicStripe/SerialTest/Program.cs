﻿//using System;
//using System.Text;
//using System.Net;
//using System.Net.Sockets;

//namespace SerialTest
//{
//    class Program
//    {
//        static void Main(string[] args)
//        {
//            IPEndPoint ip = new IPEndPoint(IPAddress.Parse("192.168.43.141"), 23);

//            Socket server = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);

//            try
//            {
//                server.Connect(ip);
//            }
//            catch (SocketException e)
//            {
//                Console.WriteLine("Unable to connect to server.");
//                return;
//            }


//                byte[] data = new byte[3];
//            while (server.Available < 3)
//            {
//                System.Threading.Thread.Sleep(100);
//            }
//                int receivedDataLength = server.Receive(data);
//                for (int i = 0; i < 3; i++)
//                {
//                    Console.WriteLine(data[i].ToString());
//                }
//                server.Send(new byte[] { 1 });


//            server.Shutdown(SocketShutdown.Both);
//            server.Close();




//            Console.Read();
//        }
//    }
//}

using System.Net.NetworkInformation;
using System.Threading;
using System.Diagnostics;
using System.Collections.Generic;
using System;

static class programm
{
    private static List<Ping> pingers = new List<Ping>();
    private static int instances = 0;

    private static object @lock = new object();

    private static int result = 0;
    private static int timeOut = 250;

    private static int ttl = 5;

    public static void Main()
    {
        string baseIP = "192.168.143.";

        Console.WriteLine("Pinging 255 destinations of D-class in {0}*", baseIP);

        CreatePingers(255);

        PingOptions po = new PingOptions(ttl, true);
        System.Text.ASCIIEncoding enc = new System.Text.ASCIIEncoding();
        byte[] data = enc.GetBytes("abababababababababababababababab");

        SpinWait wait = new SpinWait();
        int cnt = 1;

        Stopwatch watch = Stopwatch.StartNew();

        foreach (Ping p in pingers)
        {
            lock (@lock)
            {
                instances += 1;
            }

            p.SendAsync(string.Concat(baseIP, cnt.ToString()), timeOut, data, po);
            cnt += 1;
        }

        while (instances > 0)
        {
            wait.SpinOnce();
        }

        watch.Stop();

        DestroyPingers();

        Console.WriteLine("Finished in {0}. Found {1} active IP-addresses.", watch.Elapsed.ToString(), result);
        Console.ReadKey();

    }

    public static void Ping_completed(object s, PingCompletedEventArgs e)
    {
        lock (@lock)
        {
            instances -= 1;
        }

        if (e.Reply.Status == IPStatus.Success)
        {
            Console.WriteLine(string.Concat("Active IP: ", e.Reply.Address.ToString()));
            result += 1;
        }
        else
        {
            //Console.WriteLine(String.Concat("Non-active IP: ", e.Reply.Address.ToString()))
        }
    }


    private static void CreatePingers(int cnt)
    {
        for (int i = 1; i <= cnt; i++)
        {
            Ping p = new Ping();
            p.PingCompleted += Ping_completed;
            pingers.Add(p);
        }
    }

    private static void DestroyPingers()
    {
        foreach (Ping p in pingers)
        {
            p.PingCompleted -= Ping_completed;
            p.Dispose();
        }

        pingers.Clear();

    }

}