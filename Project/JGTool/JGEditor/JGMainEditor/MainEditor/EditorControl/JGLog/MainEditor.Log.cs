using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Drawing;
namespace MainEditor
{
    public partial class MainEditor : Form
    {

        private JGLog.LogLoader m_LogLoader;
        private string LogPath = "EngineCoreLog.log";

        private void EngineLog_Init()
        {
            EngineLog.WordWrap = true;
            m_LogLoader = new JGLog.LogLoader(LogPath);
            MainEditorEventHandler += EngineLog_Read;
        }
        private void EngineLog_Close()
        {
            m_LogLoader.Close();
        }
        public void EngineLog_Read()
        {
            string LogString = "NONE";
            if (m_LogLoader.Read(ref LogString))
            {
                // 일단 텍스트 추가
                LogString += Environment.NewLine;
                EngineLog.SelectionStart = EngineLog.TextLength;
                EngineLog.SelectionLength = 0;

                if (LogString.Contains("[Info]"))
                    EngineLog.SelectionColor = Color.Black;
                else if (LogString.Contains("[Warning]"))
                    EngineLog.SelectionColor = Color.Yellow;
                else if (LogString.Contains("[Error]"))
                    EngineLog.SelectionColor = Color.OrangeRed;
                else if (LogString.Contains("[Critical]"))
                    EngineLog.SelectionColor = Color.Red;

                EngineLog.AppendText(LogString);
                EngineLog.SelectionColor = EngineLog.ForeColor;
            }
        }
    }
}
