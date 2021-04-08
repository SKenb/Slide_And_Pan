using Microsoft.Win32;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;

namespace Bridge
{
    public class Python
    {
        public static IEnumerable<string> GetPythonPath()
        {
            string[] possiblePythonLocations = new string[3] {
                @"HKLM\SOFTWARE\Python\PythonCore\",
                @"HKCU\SOFTWARE\Python\PythonCore\",
                @"HKLM\SOFTWARE\Wow6432Node\Python\PythonCore\"
            };

            //Version number, install path
            Dictionary<string, string> pythonLocations = new Dictionary<string, string>();

            foreach (string possibleLocation in possiblePythonLocations)
            {
                string regKey = possibleLocation.Substring(0, 4), actualPath = possibleLocation.Substring(5);
                RegistryKey theKey = (regKey == "HKLM" ? Registry.LocalMachine : Registry.CurrentUser);
                RegistryKey theValue = theKey.OpenSubKey(actualPath);

                foreach (var v in theValue.GetSubKeyNames())
                {
                    RegistryKey productKey = theValue.OpenSubKey(v);
                    if (productKey != null)
                    {
                        string pythonExePath = null;

                        try
                        {
                            pythonExePath = productKey.OpenSubKey("InstallPath").GetValue("ExecutablePath").ToString();
                            pythonLocations.Add(v.ToString(), pythonExePath);
                        }
                        catch
                        {
                            //Install path doesn't exist
                        }

                        yield return (pythonExePath != null && pythonExePath != "") ? pythonExePath : null;
                    }
                }
            }
        }

        public static void TryExecutePython(string cmd, string args)
        {
            foreach (var pythonPath in Python.GetPythonPath())
            {
                RunPythonCmd(pythonPath, cmd, args);
                break; // TODO failure detection
            }
        }

        private static ExecutionResult RunPythonCmd(string pythonPath, string cmd, string args)
        {
            var result = Common.Execute(cmd, args, pythonPath);

            return result;
        }

    }
}
