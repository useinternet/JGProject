using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Drawing;
using JGLog;
namespace MainEditor
{
    public partial class MainEditor : Form
    {
        Log m_Log;
        Point m_LogLocatoin = new Point();
        Size m_LogSize = new Size();
        public void CreateJGLog()
        {
            m_LogLocatoin.Offset(0, 510);
            m_LogSize.Width  = 900;
            m_LogSize.Height = 270;
            // 로그 추가
            m_Log = new Log();

            // 로그 설정

            m_Log.TopLevel = false;
            m_Log.FormBorderStyle = FormBorderStyle.FixedDialog;
            m_Log.StartPosition = FormStartPosition.Manual;
            m_Log.Width = m_LogSize.Width;
            m_Log.Height = m_LogSize.Height;
            m_Log.Location = m_LogLocatoin;
            MainTabPage.Controls.Add(m_Log);
            m_Log.Show();
            MainEditorEventHandler += m_Log.Read;
        }
        
    }
}
