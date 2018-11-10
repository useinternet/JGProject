using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Threading;
using System.Windows.Forms;
using System.Runtime.InteropServices;

namespace MainEditor
{
    public delegate void MainEditorEvent();
    [StructLayout(LayoutKind.Sequential)]
    public struct PeekMsg
    {
        public IntPtr hWnd;
        public Message msg;
        public IntPtr wParam;
        public IntPtr lParam;
        public uint time;
        public System.Drawing.Point p;
    }
    public partial class MainEditor : Form
    {
        // PeekMessage 임포트
        [DllImport("User32.dll", CharSet = CharSet.Auto)]
        private static extern bool PeekMessage(out PeekMsg msg, IntPtr hWnd,
         uint messageFilterMin, uint messageFilterMax, uint flags);
        // 에디터 이벤트 핸들러
        private MainEditorEvent MainEditorEventHandler;
        // 에디터 루프
        public void EditLoop(object sender, EventArgs e)
        {
            PeekMsg msg;
            while (!PeekMessage(out msg, IntPtr.Zero, 0, 0, 0))
            {
                if (MainEditorEventHandler != null)
                {
                    MainEditorEventHandler();
                }
            }

        }
        // 각 컨트롤 초기화
        public MainEditor()
        {
            InitializeComponent();
            JGEngineCore.InitEngine(RenderPanel.Handle);
            MainEditorEventHandler += JGEngineCore.CoreRun;
            EngineLog_Init();
            Directory_Init();
        }
        // 이벤트 처리
        private void MainEditor_FormClosing(object sender, FormClosingEventArgs e)
        {
            JGEngineCore.Destroy();
            EngineLog_Close();
        }
        private void MainEditor_SizeChanged(object sender, EventArgs e)
        {

        }

        private void SearchBox_TextChanged(object sender, EventArgs e)
        {

        }


    }
}
