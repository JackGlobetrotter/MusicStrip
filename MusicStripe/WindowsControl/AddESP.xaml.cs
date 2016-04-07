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

// The Content Dialog item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

namespace WindowsControl
{
    public sealed partial class AddESP : ContentDialog
    {
        public AddESP()
        {
            this.InitializeComponent();
        }
        private void BackButton_Click(object sender, RoutedEventArgs e)
        {
            if (rootPivot.SelectedIndex > 0)
            {
                // If not at the first item, go back to the previous one.
                rootPivot.SelectedIndex -= 1;
            }
            else
            {
                // The first PivotItem is selected, so loop around to the last item.
                rootPivot.SelectedIndex = rootPivot.Items.Count - 1;
            }
        }

        private void NextButton_Click(object sender, RoutedEventArgs e)
        {
            if (rootPivot.SelectedIndex < rootPivot.Items.Count - 1)
            {
                // If not at the last item, go to the next one.
                rootPivot.SelectedIndex += 1;
            }
            else
            {
                // The last PivotItem is selected, so loop around to the first item.
                rootPivot.SelectedIndex = 0;
            }
        }
        private void ContentDialog_PrimaryButtonClick(ContentDialog sender, ContentDialogButtonClickEventArgs args)
        {
        }

        private void ContentDialog_SecondaryButtonClick(ContentDialog sender, ContentDialogButtonClickEventArgs args)
        {
        }
    }
}
