# Copyright (c) 2024 Roger Brown.
# Licensed under the MIT License.

OBJDIR=obj\$(VSCMD_ARG_TGT_ARCH)
BINDIR=bin\$(VSCMD_ARG_TGT_ARCH)
DLLNAME=displib

!IF "$(VSCMD_ARG_TGT_ARCH)" == "arm"
MIDL_ARCH=arm32
STUB_DEFINE=/D_ARM_
!ELSE
!IF "$(VSCMD_ARG_TGT_ARCH)" == "arm64"
MIDL_ARCH=arm64
!ELSE
!IF "$(VSCMD_ARG_TGT_ARCH)" == "x64"
MIDL_ARCH=x64
!ELSE
MIDL_ARCH=win32
!ENDIF
!ENDIF
!ENDIF

all: $(BINDIR)\$(DLLNAME).dll

$(OBJDIR)\dlldata.c $(OBJDIR)\$(DLLNAME)_h.h $(OBJDIR)\$(DLLNAME)_i.c $(OBJDIR)\$(DLLNAME)_p.c $(OBJDIR)\$(DLLNAME).tlb: $(DLLNAME).idl $(OBJDIR)
	midl $(DLLNAME).idl /client none /server none /tlb $(DLLNAME).tlb /h $(DLLNAME)_h.h /out $(OBJDIR) /env $(MIDL_ARCH)

clean:
	if exist $(OBJDIR) rmdir /q /s $(OBJDIR)
	if exist $(BINDIR) rmdir /q /s $(BINDIR)

$(BINDIR)\$(DLLNAME).dll: $(OBJDIR)\dlldata.obj $(OBJDIR)\$(DLLNAME)_p.obj $(OBJDIR)\$(DLLNAME)_i.obj $(OBJDIR)\$(DLLNAME).res $(DLLNAME).def $(BINDIR)
	cl /LD /Fe$@ /MT  $(OBJDIR)\dlldata.obj $(OBJDIR)\$(DLLNAME)_p.obj $(OBJDIR)\$(DLLNAME)_i.obj $(OBJDIR)\$(DLLNAME).res ole32.lib oleaut32.lib advapi32.lib rpcrt4.lib /link /INCREMENTAL:NO /PDB:NONE /DEF:$(DLLNAME).def /SUBSYSTEM:CONSOLE
	if not "$(CertificateThumbprint)"=="" signtool sign /a /sha1 $(CertificateThumbprint) /fd SHA256 /t http://timestamp.digicert.com $@

$(OBJDIR)\$(DLLNAME)_i.obj: $(OBJDIR)\$(DLLNAME)_i.c
	cl /c /Fo$@ $(OBJDIR)\$(DLLNAME)_i.c /I$(OBJDIR)

$(OBJDIR)\$(DLLNAME)_p.obj: $(OBJDIR)\$(DLLNAME)_p.c
	cl /c /Fo$@ $(OBJDIR)\$(DLLNAME)_p.c /I$(OBJDIR) /DWIN32 $(STUB_DEFINE)

$(OBJDIR)\dlldata.obj: $(OBJDIR)\dlldata.c
	cl /c /Fo$@ $(OBJDIR)\dlldata.c /I$(OBJDIR) /DWIN32 $(STUB_DEFINE)

$(OBJDIR)\$(DLLNAME).res: $(DLLNAME).rc $(OBJDIR)\$(DLLNAME).tlb
	rc /r  $(RCFLAGS) /i $(OBJDIR) /fo$@ $(DLLNAME).rc

$(BINDIR) $(OBJDIR):
	mkdir $@
