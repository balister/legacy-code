# Microsoft Developer Studio Project File - Name="ossieparser" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=ossieparser - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "ossieparser.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ossieparser.mak" CFG="ossieparser - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ossieparser - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "ossieparser - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ossieparser - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "OSSIEPARSER_EXPORTS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "OSSIEPARSER_EXPORTS" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386

!ELSEIF  "$(CFG)" == "ossieparser - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "OSSIEPARSER_EXPORTS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GR /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "OSSIEPARSER_EXPORTS" /FR /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib aced.lib taod.lib xerces-c_2.lib TAO_CosNamingd.lib TAO_PortableServerd.lib /nologo /dll /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "ossieparser - Win32 Release"
# Name "ossieparser - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\ossieidl\cf.idl

!IF  "$(CFG)" == "ossieparser - Win32 Release"

!ELSEIF  "$(CFG)" == "ossieparser - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Custom build IDL
InputDir=\ossie\ossieidl
InputPath=..\ossieidl\cf.idl
InputName=cf

BuildCmds= \
	tao_idl $(InputPath) -o $(InputDir) -I $(InputDir)

"$(InputName)s.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName)s.i" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName)s.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName)c.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName)c.i" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName)c.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ossieidl\cfC.cpp
# End Source File
# Begin Source File

SOURCE=.\ComponentAssemblyParser.cpp
# End Source File
# Begin Source File

SOURCE=.\ComponentInstantiation.cpp
# End Source File
# Begin Source File

SOURCE=.\ComponentPlacement.cpp
# End Source File
# Begin Source File

SOURCE=.\ComponentSupportedInterface.cpp
# End Source File
# Begin Source File

SOURCE=.\Connection.cpp
# End Source File
# Begin Source File

SOURCE=.\DCDComponentInstantiation.cpp
# End Source File
# Begin Source File

SOURCE=.\DCDComponentPlacement.cpp
# End Source File
# Begin Source File

SOURCE=.\DCDConnectionParser.cpp
# End Source File
# Begin Source File

SOURCE=.\DCDInstantiationProperty.cpp
# End Source File
# Begin Source File

SOURCE=.\DCDParser.cpp
# End Source File
# Begin Source File

SOURCE=.\DMDParser.cpp
# End Source File
# Begin Source File

SOURCE=.\ExternalPort.cpp
# End Source File
# Begin Source File

SOURCE=.\FindBy.cpp
# End Source File
# Begin Source File

SOURCE=.\InstantiationProperty.cpp
# End Source File
# Begin Source File

SOURCE=.\ORB_WRAP.cpp
# End Source File
# Begin Source File

SOURCE=.\ossieparser.cpp
# End Source File
# Begin Source File

SOURCE=.\Port.cpp
# End Source File
# Begin Source File

SOURCE=.\PRFParser.cpp
# End Source File
# Begin Source File

SOURCE=.\PRFSimpleProperty.cpp
# End Source File
# Begin Source File

SOURCE=.\ProvidesPort.cpp
# End Source File
# Begin Source File

SOURCE=.\SADComponentInstantiation.cpp
# End Source File
# Begin Source File

SOURCE=.\SADComponentPlacement.cpp
# End Source File
# Begin Source File

SOURCE=.\SADHostCollocation.cpp
# End Source File
# Begin Source File

SOURCE=.\SADParser.cpp
# End Source File
# Begin Source File

SOURCE=.\SCDParser.cpp
# End Source File
# Begin Source File

SOURCE=.\SPDAuthor.cpp
# End Source File
# Begin Source File

SOURCE=.\SPDImplementation.cpp
# End Source File
# Begin Source File

SOURCE=.\SPDParser.cpp
# End Source File
# Begin Source File

SOURCE=.\SPDUsesDevice.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\UsesPort.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\CFC.h
# End Source File
# Begin Source File

SOURCE=.\ComponentAssemblyParser.h
# End Source File
# Begin Source File

SOURCE=.\ComponentInstantiation.h
# End Source File
# Begin Source File

SOURCE=.\ComponentPlacement.h
# End Source File
# Begin Source File

SOURCE=.\ComponentSupportedInterface.h
# End Source File
# Begin Source File

SOURCE=.\Connection.h
# End Source File
# Begin Source File

SOURCE=.\DCDComponentInstantiation.h
# End Source File
# Begin Source File

SOURCE=.\DCDComponentPlacement.h
# End Source File
# Begin Source File

SOURCE=.\DCDConnectionParser.h
# End Source File
# Begin Source File

SOURCE=.\DCDInstantiationProperty.h
# End Source File
# Begin Source File

SOURCE=.\DCDParser.h
# End Source File
# Begin Source File

SOURCE=.\DMDParser.h
# End Source File
# Begin Source File

SOURCE=.\ExternalPort.h
# End Source File
# Begin Source File

SOURCE=.\FindBy.h
# End Source File
# Begin Source File

SOURCE=.\InstantiationProperty.h
# End Source File
# Begin Source File

SOURCE=.\ORB_WRAP.h
# End Source File
# Begin Source File

SOURCE=.\ossieparser.h
# End Source File
# Begin Source File

SOURCE=.\Port.h
# End Source File
# Begin Source File

SOURCE=.\PRFParser.h
# End Source File
# Begin Source File

SOURCE=.\PRFSimpleProperty.h
# End Source File
# Begin Source File

SOURCE=.\ProvidesPort.h
# End Source File
# Begin Source File

SOURCE=.\SADComponentInstantiation.h
# End Source File
# Begin Source File

SOURCE=.\SADComponentPlacement.h
# End Source File
# Begin Source File

SOURCE=.\SADHostCollocation.h
# End Source File
# Begin Source File

SOURCE=.\SADParser.h
# End Source File
# Begin Source File

SOURCE=.\SCDParser.h
# End Source File
# Begin Source File

SOURCE=.\SPDAuthor.h
# End Source File
# Begin Source File

SOURCE=.\SPDImplementation.h
# End Source File
# Begin Source File

SOURCE=.\SPDParser.h
# End Source File
# Begin Source File

SOURCE=.\SPDUsesDevice.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\UsesPort.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
