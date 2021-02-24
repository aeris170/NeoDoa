using System;
using System.Runtime.InteropServices;

namespace EngineInterop {

    public class Window {

        internal C.Window window;

        public int Width { get => window.Width; }
        public int Height { get => window.Height; }
        public string Title { get => Marshal.PtrToStringAnsi(window.Title); }

        internal Window() {}
    }

    #region C
    internal partial class C {

        [StructLayout(LayoutKind.Sequential)]
        public struct Window {
            public int Width;
            public int Height;
            public IntPtr Title;
        }
    }
    #endregion
}