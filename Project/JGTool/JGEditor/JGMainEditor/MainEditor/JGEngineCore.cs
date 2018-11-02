using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Runtime.InteropServices;
namespace MainEditor
{
    public class JGEngineCore
    {
#if DEBUG
        [DllImport("EngineBridge_d.dll")]
        public static extern void InitEngine(IntPtr hWnd);
        [DllImport("EngineBridge_d.dll")]
        public static extern void Run();
        [DllImport("EngineBridge_d.dll")]
        public static extern void End();
        [DllImport("EngineBridge_d.dll")]
        public static extern void EG2ED_SendCommand(char[] Command, IntPtr Data, uint Size);
        [DllImport("EngineBridge_d.dll")]
        public static extern void ED2EG_SendCommand(char[] Command, IntPtr Data, uint Size);
#else
        [DllImport("EngineBridge.dll")]
        public static extern bool InitEngine(IntPtr hWnd);
        [DllImport("EngineBridge.dll")]
        public static extern void Run();
        [DllImport("EngineBridge.dll")]
        public static extern void End();
        [DllImport("EngineBridge.dll")]
        public static extern void EG2ED_SendCommand(char[] Command, IntPtr Data, uint Size);
        [DllImport("EngineBridge.dll")]
        public static extern void ED2EG_SendCommand(char[] Command, IntPtr Data, uint Size);
#endif
        static public void CoreInit(IntPtr hWnd)
        {
            InitEngine(hWnd);
        }
        static public void CoreRun()
        {
            Run();
        }
        static public void Destroy()
        {
            End();
        }
        static public void SendCommand(char[] Command, IntPtr Data, uint Size)
        {
            ED2EG_SendCommand(Command, Data, Size);
        }
        static public void ReceiveCommand(char[] Command, IntPtr Data, uint Size)
        {
            EG2ED_SendCommand(Command, Data, Size);
        }
    }
}
