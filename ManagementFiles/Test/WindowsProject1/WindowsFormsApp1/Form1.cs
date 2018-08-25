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



/*namespace DirectXinForms
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
        }
        private void button1_Click(object sender, EventArgs e)
        {
            Dengine.InitD3D(panel1.Handle, panel1.Width, panel1.Height);
            Dengine.RenderFrame();
        }
        private void Form1_FormClosing(object sender, FormClosingEventArgs e)
        {
            Dengine.CleanD3D();
        }
    }
    public class Dengine
    {
        [DllImport("DirectXENGINEDLL.dll")]
        public static extern void InitD3D(IntPtr window, int Width, int Height);
        [DllImport("DirectXENGINEDLL.dll")]
        public static extern void RenderFrame();
        [DllImport("DirectXENGINEDLL.dll")]
        public static extern void CleanD3D();
    }
}*/
namespace WindowsFormsApp1
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
            Dengine.InitD3D(panel1.Handle, panel1.Width, panel1.Height);

        }

        private void Form1_Load(object sender, EventArgs e)
        {
  
        }

        private void Form1_FormClosing(object sender, FormClosingEventArgs e)
        {
            Dengine.CleanD3D();
        }

        private void panel1_Paint(object sender, PaintEventArgs e)
        {

        }

        private void button1_Click(object sender, EventArgs e)
        {
            Dengine.RenderFrame();
        }
    }

    public class Dengine
    {
        [DllImport("Dll1.dll")]
        public static extern void InitD3D(IntPtr window, int Width, int Height);
        [DllImport("Dll1.dll")]
        public static extern void RenderFrame();
        [DllImport("Dll1.dll")]
        public static extern void CleanD3D();
    }

}
