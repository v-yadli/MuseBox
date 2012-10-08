using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;

namespace MuseBox
{
    unsafe class Memory
    {
        [DllImport("msvcrt.dll")]
        public static extern void* malloc(uint size);
        [DllImport("msvcrt.dll")]
        public static extern void free(void* ptr);
        [DllImport("msvcrt.dll")]
        public static extern void memcpy(void* dest, void* src, uint size);
    }
}
