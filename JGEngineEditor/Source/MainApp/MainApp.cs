using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Threading;
using System.Windows.Forms;
using System.Runtime.InteropServices;

namespace JGEngineEditor.Source.MainApp
{
    class MainApp : IDisposable
    {
        private MainForm m_MainForm = null;

        public bool InitializeApplication(MainForm topLevelForm)
        {
            this.m_MainForm = topLevelForm;
            SampleEngine.InitEngine(m_MainForm.RenderBox.Handle, m_MainForm.RenderBox.Width, m_MainForm.RenderBox.Height);
            return true;
        }

        public void MainLoop()
        {
            SampleEngine.Start();
        }

        public void Dispose()
        {
            SampleEngine.End();
        }
    }
    public class SampleEngine
    {
        [DllImport("InterpBridge.dll")]
        public static extern void InitEngine(IntPtr window, int Width, int Height);
        [DllImport("InterpBridge.dll")]
        public static extern void Start();
        [DllImport("InterpBridge.dll")]
        public static extern void End();
    }

}
