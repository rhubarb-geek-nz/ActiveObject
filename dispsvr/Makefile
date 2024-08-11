# Copyright (c) 2024 Roger Brown.
# Licensed under the MIT License.

OBJDIR=obj\$(VSCMD_ARG_TGT_ARCH)
BINDIR=bin\$(VSCMD_ARG_TGT_ARCH)
APPNAME=dispsvr

all: $(BINDIR)\$(APPNAME).exe

clean:
	if exist $(OBJDIR) rmdir /q /s $(OBJDIR)
	if exist $(BINDIR) rmdir /q /s $(BINDIR)

$(BINDIR)\$(APPNAME).exe: $(OBJDIR)\$(APPNAME).obj $(OBJDIR)\$(APPNAME).res $(BINDIR) $(OBJDIR)\displib_i.obj
	cl /Fe$@ /MT $(OBJDIR)\$(APPNAME).obj $(OBJDIR)\displib_i.obj $(OBJDIR)\$(APPNAME).res ole32.lib oleaut32.lib advapi32.lib user32.lib /link /INCREMENTAL:NO /PDB:NONE /SUBSYSTEM:CONSOLE /MANIFEST
	mt -manifest $(APPNAME).manifest $@.manifest -outputresource:$@;#1
	del $@.manifest
	if not "$(CertificateThumbprint)"=="" signtool sign /a /sha1 $(CertificateThumbprint) /fd SHA256 /t http://timestamp.digicert.com $@

$(OBJDIR)\$(APPNAME).obj: $(APPNAME).c $(OBJDIR)
	cl /c /Fo$@ $(APPNAME).c /I..\displib\$(OBJDIR) /DWIN32_LEAN_AND_MEAN /DCOBJMACROS

$(OBJDIR)\displib_i.obj: ..\displib\$(OBJDIR)\displib_i.c $(OBJDIR)
	cl /c /Fo$@ ..\displib\$(OBJDIR)\displib_i.c /DWIN32

$(OBJDIR)\$(APPNAME).res: $(APPNAME).rc
	rc /r $(RCFLAGS) /fo$@ $(APPNAME).rc

$(BINDIR) $(OBJDIR):
	mkdir $@
