using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace MainEditor.JGLog
{
    public class LogLoader
    {
        FileStream LogFileStream;
        StreamReader LogReader;
        public LogLoader(string LogPath)
        {
#if DEBUG
            LogFileStream = new FileStream(LogPath, FileMode.Open, FileAccess.Read, FileShare.ReadWrite);
            LogReader = new StreamReader(LogFileStream);
#endif
        }
        public bool Read(ref string log)
        {
#if DEBUG
            if (LogReader.EndOfStream)
            {
                return false;
            }
            else
            {
                log = LogReader.ReadLine();
                return true;
            }
#else
            return false;
#endif
        }
        public void Close()
        {
            LogFileStream.Close();
        }
    }
}
