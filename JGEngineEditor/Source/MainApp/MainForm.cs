using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

// UI using선언

namespace JGEngineEditor.Source.MainApp
{
    public partial class MainForm : Form
    {
        private RenderBoxController RenderBoxCtr = new RenderBoxController();
        public MainForm()
        {
            InitializeComponent();
            RenderBoxCtr.ConnectRenderBox(ref(RenderBox));
        }
        //
        private void MainForm_Load(object sender, EventArgs e)
        {
            this.WindowState = FormWindowState.Maximized;
        }
        // 마우스 이동 
        private void MainForm_MouseMove(object sender, MouseEventArgs e)
        {
            RenderBoxCtr.MousePointCollisonCheck(e);
            DebugTextBox.Text = RenderBoxCtr.GetCurrentCollisionType().ToString();
        }

        private void MainForm_MouseDown(object sender, MouseEventArgs e)
        {
            RenderBoxCtr.ChangeBoxSizeStart(e);
            DebugTextBox.Text = RenderBoxCtr.GetCurrentCollisionType().ToString();
        }

        private void MainForm_MouseUp(object sender, MouseEventArgs e)
        {
            LogBox.AppendText("ㅎㅇ");
            RenderBoxCtr.ChangeBoxSizeEnd();
        }
    }
}
