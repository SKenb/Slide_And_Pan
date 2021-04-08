using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Bridge
{
    public class ExecutionResult
    {
        public string Ouput { get; set; } = "";
        public string Error { get; set; } = "";
        public Exception ExecutionError { get; set; } = null;
    }

    public class Common
    {
        
        public static ExecutionResult Execute(string cmd, string args = "", string pathToProgram = "cmd.exe")
        {
            var result = new ExecutionResult();

            ProcessStartInfo startInfo = new ProcessStartInfo();

            startInfo.FileName = pathToProgram;
            startInfo.Arguments = string.Format("{0} {1}", cmd, args);
            startInfo.UseShellExecute = false;
            startInfo.RedirectStandardOutput = true;
            startInfo.RedirectStandardError = true;
            startInfo.CreateNoWindow = true;

            using (Process process = Process.Start(startInfo))
            {
                using (StreamReader reader = process.StandardOutput) result.Ouput = reader.ReadToEnd();
                using (StreamReader reader = process.StandardError) result.Error = reader.ReadToEnd();
            }

            return result;
        }
    }
}
