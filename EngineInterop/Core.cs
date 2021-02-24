using System;
using System.Runtime.InteropServices;

namespace EngineInterop {

    public class Core : IDisposable {

        internal C.Core core;

        private readonly Window window;

        public Window Window { get => window; }

        public Core(int width, int height, string title) : this(width, height, title, false) {}

        public Core(int width, int height, string title, bool isFullscreen) {
            core = (C.Core) Marshal.PtrToStructure(C.NewCore(width, height, title, isFullscreen), typeof(C.Core));
            window = new Window {
                window = (C.Window) Marshal.PtrToStructure(core.Window, typeof(C.Window))
            };
        }

        ~Core() { Dispose(); }

        public void Start() { C.StartCore(ref core); }

        public void Dispose() {
            C.FreeCore(ref core);
            GC.SuppressFinalize(this);
        }
    };

    #region C
    internal partial class C {

        [StructLayout(LayoutKind.Sequential)]
        public struct Core {
            internal IntPtr Window;
        }

        [DllImport("../Engine.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr NewCore(int width, int height, [MarshalAs(UnmanagedType.LPStr)] string title, bool isFullscreen);

        [DllImport("../Engine.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern void StartCore(ref Core core);

        [DllImport("../Engine.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern void FreeCore(ref Core core);
    };
    #endregion
}
