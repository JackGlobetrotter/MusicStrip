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
    public delegate void StartupDataHandel(object sender, EventArgs e);

    /// <summary>
    /// Une page vide peut être utilisée seule ou constituer une page de destination au sein d'un frame.
    /// </summary>

        public sealed partial class ModuleSettings : UserControl
    {
        public event StartupDataHandel StartupDataSet;
        public ModuleSettings()
        {
            this.InitializeComponent();
        }

        private void WriteStartupData_Click(object sender, RoutedEventArgs e)
        {
            StartupDataSet(this, EventArgs.Empty);
        }
    }
}
