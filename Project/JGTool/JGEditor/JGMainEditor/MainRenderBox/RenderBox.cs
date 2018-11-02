using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Runtime.InteropServices;
using System.Threading;
namespace MainRenderBox
{
    public delegate void RenderBoxPaintEvent();
    public partial class RenderBox : Form
    {
        public RenderBox()
        {
            InitializeComponent();
        }
        public IntPtr PannelHandle
        {
            get { return RenderPannel.Handle; }
        }

        private void RenderBox_Click(object sender, EventArgs e)
        {
            this.TopMost = true;
        }
    }
}
