using System;
using System.Diagnostics;
using System.IO;
using System.Windows;

namespace Bridge.Hardware
{
    class Programmer
    {
        public static void Upload()
        {
            /*foreach(var path in Python.GetPythonPath())
                MessageBox.Show(path?? "Null");*/

            Python.TryExecutePython("where esptool", "");
        }


        
    }
}
