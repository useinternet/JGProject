using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace JGLog
{
    public partial class Log : Form
    {
        LogLoader m_Loader;
        string LogPath = "EngineCoreLog.log";

        public Log()
        {
            InitializeComponent();
            LogBox.WordWrap = true;
            m_Loader = new LogLoader(LogPath);

        }
        public void Read()
        {
            string LogString = "NONE";
            if (m_Loader.Read(ref LogString))
            {
                // 일단 텍스트 추가
                LogString += Environment.NewLine;
                LogBox.SelectionStart  = LogBox.TextLength;
                LogBox.SelectionLength = 0;

                if (LogString.Contains("[Info]"))
                    LogBox.SelectionColor = Color.Black;
                else if (LogString.Contains("[Warning]"))
                    LogBox.SelectionColor = Color.Yellow;
                else if (LogString.Contains("[Error]"))
                    LogBox.SelectionColor = Color.OrangeRed;
                else if (LogString.Contains("[Critical]"))
                    LogBox.SelectionColor = Color.Red;

                LogBox.AppendText(LogString);
                LogBox.SelectionColor = LogBox.ForeColor;
            }
        }
        private void Log_FormClosing(object sender, FormClosingEventArgs e)
        {
            m_Loader.Close();
        }

        private void Log_Click(object sender, EventArgs e)
        {
            this.TopMost = true;
        }
    }
}
