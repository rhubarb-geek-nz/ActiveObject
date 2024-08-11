# rhubarb-geek-nz/ActiveObject

Demonstration of local server COM object registered as an active object.

[displib.idl](displib/displib.idl) defines the dual-interface for a simple local server. Used to create a proxy/stub library.

[dispsvr.c](dispsvr/dispsvr.c) implements the interface and published with [RegisterActiveObject](https://learn.microsoft.com/en-us/windows/win32/api/oleauto/nf-oleauto-registeractiveobject).

[dispapp.cpp](dispapp/dispapp.cpp) calls [GetActiveObject](https://learn.microsoft.com/en-us/windows/win32/api/oleauto/nf-oleauto-getactiveobject) to get server and uses it to get a message to display.

[package.ps1](package.ps1) is used to automate the building of multiple architectures.
