using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using MainRenderBox;
using JGLog;
namespace MainEditor
{
    public partial class MainEditor : Form
    {
        RenderBox m_RenderBox;
        Log       m_Log;
        public MainEditor()
        {
            InitializeComponent();
        }
        private void MainEditor_Shown(object sender, EventArgs e)
        {
            // 렌더 박스 추가
            m_RenderBox = new RenderBox();
            m_RenderBox.Width  = 800;
            m_RenderBox.Height = 600;
            m_RenderBox.TopLevel = false;
            m_RenderBox.Show();
            MainTabPage.Controls.Add(m_RenderBox);
            // 로그 추가
            m_Log = new Log();
            m_Log.Dock = DockStyle.Right;
            m_Log.TopLevel = false;
            m_Log.Show();
            MainTabPage.Controls.Add(m_Log);
        }
    }
}
