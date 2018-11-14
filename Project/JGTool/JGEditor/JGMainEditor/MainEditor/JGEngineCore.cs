using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Runtime.InteropServices;
namespace MainEditor
{
    [StructLayout(LayoutKind.Sequential,Pack = 1)]
    struct DxWindowConfigCpp
    {
        public IntPtr hWnd;
        [MarshalAs(UnmanagedType.Bool)]
        public bool bFullScreen;
        [MarshalAs(UnmanagedType.I4)]
        public int ScreenWidth;
        [MarshalAs(UnmanagedType.I4)]
        public int ScreenHeight;
        [MarshalAs(UnmanagedType.R4)]
        public float Fov;
        [MarshalAs(UnmanagedType.R4)]
        public float FarZ;
        [MarshalAs(UnmanagedType.R4)]
        public float NearZ;
    }
    public class JGHandle
    {
        static public IntPtr GetHandle(object obj)
        {
            IntPtr data = Marshal.AllocHGlobal(Marshal.SizeOf(obj));
            Marshal.StructureToPtr(obj, data, true);
            return data;
        }
        static public void ReleaseHandle(IntPtr data)
        {
            Marshal.FreeHGlobal(data);
        }
    }
    public enum ED2EGCommand
    {
        ed_AddDxWindow
    }
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
        private static extern void EG2ED_SendCommand(uint Command, [In] IntPtr Data, uint Size);
        [DllImport("EngineBridge_d.dll")]
        private static extern void ED2EG_SendCommand(uint Command, [In] IntPtr Data, uint Size);
#else
        [DllImport("EngineBridge.dll")]
        public static extern bool InitEngine(IntPtr hWnd);
        [DllImport("EngineBridge.dll")]
        public static extern void Run();
        [DllImport("EngineBridge.dll")]
        public static extern void End();
        [DllImport("EngineBridge.dll")]
        public static extern void EG2ED_SendCommand(uint Command, IntPtr Data, uint Size);
        [DllImport("EngineBridge.dll")]
        public static extern void ED2EG_SendCommand(uint Command, IntPtr Data, uint Size);
#endif
        static public void SendCommand(ED2EGCommand command, object Data)
        {
            IntPtr ptr = JGHandle.GetHandle(Data);
            ED2EG_SendCommand((uint)command, ptr, (uint)Marshal.SizeOf(Data));
            JGHandle.ReleaseHandle(ptr);
        }
        static public void ReceiveCommand(ED2EGCommand command, IntPtr Data, uint Size)
        {
            EG2ED_SendCommand((uint)command, Data, Size);
        }
    }
}
