/***********************************
 * Copyright (c) 2024 Roger Brown.
 * Licensed under the MIT License.
 ****/

using System;
using System.Runtime.InteropServices;

namespace RhubarbGeekNzActiveObject
{
    internal class Program
    {
        static void Main(string[] args)
        {
            Guid iid = typeof(IHelloWorld).GUID;
            CoRegisterPSClsid(ref iid, ref iid);

            Guid clsid = new Guid("{1cb0313b-aec5-407a-b4be-058e5e765a6d}");
            GetActiveObject(ref clsid, IntPtr.Zero, out object active);

            IHelloWorld helloWorld = active as IHelloWorld;

            string result = helloWorld.GetMessage(1);

            Console.WriteLine($"{result}");
        }

        [DllImport("oleaut32.dll", PreserveSig = false)]
        private static extern void GetActiveObject(ref Guid rclsid, IntPtr pvReserved, [MarshalAs(UnmanagedType.IUnknown)] out Object ppunk);

        [DllImport("ole32.dll", PreserveSig = false)]
        private static extern void CoRegisterPSClsid(ref Guid rclsid, ref Guid riid);
    }
}
