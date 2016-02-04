#!/usr/bin/python
# -*- coding: utf-8 -*-

import sys
sys.path.insert(0, '../build')
from packageparser import PackageParser, Package
import xml.sax
import subprocess
import os
import shutil
import platform

ROOT_PATH = os.path.abspath('../../')
PKG_PATH = os.path.abspath('../build/package.xml')


class Nsis:

    SCRIPT_HEADER = \
        '''
# Auto-generated by EclipseNSIS Script Wizard
# Sep 17, 2012 11:27:04 PM
'''

    SCRIPT_NAME = 'Name "<NAME>"'
    SCRIPT_START_MENU = \
        '!define MUI_STARTMENUPAGE_DEFAULTFOLDER "<NAME>"\n'
    SCRIPT_BODY_1 = \
        '''
RequestExecutionLevel admin

# General Symbol Definitions
!define REGKEY "SOFTWARE\$(^Name)"

# MUI Symbol Definitions
!define MUI_ICON "${NSISDIR}\Contrib\Graphics\Icons\classic-install.ico"
!define MUI_FINISHPAGE_NOAUTOCLOSE
!define MUI_STARTMENUPAGE_REGISTRY_ROOT HKLM
!define MUI_STARTMENUPAGE_NODISABLE
!define MUI_STARTMENUPAGE_REGISTRY_KEY ${REGKEY}
!define MUI_STARTMENUPAGE_REGISTRY_VALUENAME StartMenuGroup
'''

    SCRIPT_BODY_2 = \
        '''
!define MUI_UNICON "${NSISDIR}\Contrib\Graphics\Icons\classic-uninstall.ico"
!define MUI_UNFINISHPAGE_NOAUTOCLOSE

# Included files
!include Sections.nsh
!include MUI2.nsh

# Variables
Var StartMenuGroup

# Installer pages
!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_STARTMENU Application $StartMenuGroup
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH
!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES

# Installer languages
!insertmacro MUI_LANGUAGE English

# Installer attributes
OutFile setup.exe
'''
    SCRIPT_BODY_3 = \
        '''
CRCCheck on
XPStyle on
ShowInstDetails show
InstallDirRegKey HKLM "${REGKEY}" Path
ShowUninstDetails show
# Installer sections
Section -Main SEC0000
'''

    SCRIPT_FILE_LINE = 'File "<FILE>"\n'
    SCRIPT_INSTALL_LINE = 'InstallDir $PROGRAMFILES64"<NAME>"\n'
    SCRIPT_INSTALL_SECTION_LIB = \
        '''
    SetOutPath $INSTDIR\lib
    SetOverwrite on
'''

    SCRIPT_INSTALL_SECTION_DOC = \
        '''
    SetOutPath $INSTDIR\doc
    SetOverwrite on
'''

    SCRIPT_INSTALL_SECTION_INC = \
        '''
    SetOutPath $INSTDIR\inc
    SetOverwrite on
'''

    SCRIPT_INSTALL_SECTION_EXE = \
        '''    
    SetOutPath $INSTDIR
    SetOverwrite on
'''
    SCRIPT_FOOTER = \
        '''
    WriteRegStr HKLM "${REGKEY}\Components" Main 1
SectionEnd

Section -post SEC0001
    WriteRegStr HKLM "${REGKEY}" Path $INSTDIR
    SetOutPath $INSTDIR
    WriteUninstaller $INSTDIR\uninstall.exe
    !insertmacro MUI_STARTMENU_WRITE_BEGIN Application
    SetOutPath $SMPROGRAMS\$StartMenuGroup
    CreateShortcut "$SMPROGRAMS\$StartMenuGroup\Uninstall $(^Name).lnk" $INSTDIR\uninstall.exe
    !insertmacro MUI_STARTMENU_WRITE_END
    WriteRegStr HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\$(^Name)" DisplayName "$(^Name)"
    WriteRegStr HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\$(^Name)" URLInfoAbout "${URL}"
    WriteRegStr HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\$(^Name)" DisplayIcon $INSTDIR\uninstall.exe
    WriteRegStr HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\$(^Name)" UninstallString $INSTDIR\uninstall.exe
    WriteRegDWORD HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\$(^Name)" NoModify 1
    WriteRegDWORD HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\$(^Name)" NoRepair 1
SectionEnd

# Macro for selecting uninstaller sections
!macro SELECT_UNSECTION SECTION_NAME UNSECTION_ID
    Push $R0
    ReadRegStr $R0 HKLM "${REGKEY}\Components" "${SECTION_NAME}"
    StrCmp $R0 1 0 next${UNSECTION_ID}
    !insertmacro SelectSection "${UNSECTION_ID}"
    GoTo done${UNSECTION_ID}
next${UNSECTION_ID}:
    !insertmacro UnselectSection "${UNSECTION_ID}"
done${UNSECTION_ID}:
    Pop $R0
!macroend

# Uninstaller sections
Section /o -un.Main UNSEC0000
    DeleteRegValue HKLM "${REGKEY}\Components" Main
SectionEnd

Section -un.post UNSEC0001
    DeleteRegKey HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\$(^Name)"
    Delete /REBOOTOK "$SMPROGRAMS\$StartMenuGroup\Uninstall $(^Name).lnk"
    Delete /REBOOTOK $INSTDIR\uninstall.exe
    DeleteRegValue HKLM "${REGKEY}" StartMenuGroup
    DeleteRegValue HKLM "${REGKEY}" Path
    DeleteRegKey /IfEmpty HKLM "${REGKEY}\Components"
    DeleteRegKey /IfEmpty HKLM "${REGKEY}"
    RmDir /r /REBOOTOK $SMPROGRAMS\$StartMenuGroup
    RmDir /r /REBOOTOK $INSTDIR
SectionEnd

# Installer functions
Function .onInit
    InitPluginsDir
FunctionEnd

# Uninstaller functions
Function un.onInit
    ReadRegStr $INSTDIR HKLM "${REGKEY}" Path
    !insertmacro MUI_STARTMENU_GETFOLDER Application $StartMenuGroup
    !insertmacro SELECT_UNSECTION Main ${UNSEC0000}
FunctionEnd
'''

    def __init__(self, miniatroot, pkgname):

        self.miniatroot = os.path.normpath(miniatroot)
        self.rootdir = os.path.join(miniatroot, 'out/package/windows-'
                                    + platform.machine())
        self.rootdir = os.path.normpath(self.rootdir)
        try:
            os.makedirs(os.path.join(self.rootdir))
        except:
            pass

        self.outfile = file(os.path.join(self.rootdir, 'miniat.nsi'),
                            'w+')

        self.pkgname = pkgname
        self.outfile.write(Nsis.SCRIPT_HEADER)

        name = str(self.SCRIPT_NAME)
        name = name.replace('<NAME>', self.pkgname)
        self.outfile.write(name + '\n')

        self.outfile.write(Nsis.SCRIPT_BODY_1)

        install = str(self.SCRIPT_INSTALL_LINE)
        install = install.replace('<NAME>', self.pkgname)
        self.outfile.write(install)
        self.outfile.write(Nsis.SCRIPT_BODY_2)

        startmenu = str(self.SCRIPT_START_MENU)
        startmenu = startmenu.replace('<NAME>', self.pkgname)
        self.outfile.write(startmenu)
        self.outfile.write(Nsis.SCRIPT_BODY_3)

    def package(self, items):

        extensions = dict()
        filetype = dict()

        s = items[Package.extensionTag]

        for x in s:
            plat = x['os']
            if plat == sys.platform:

                sfx = ''
                prx = ''
                if 'suffix' in x:
                    sfx = x['suffix']
                if 'prefix' in x:
                    prx = x['prefix']

                extensions[x['type']] = {'suffix': sfx, 'prefix': prx}

        for name in items:

            if name == Package.extensionTag:
                continue

            for kvp in items[name]:
                path = kvp['path']
                fname = kvp['name']

                sfx = ''
                pfx = ''
                if name in extensions:
                    sfx = extensions[name]['suffix']
                    pfx = extensions[name]['prefix']

                path = os.path.join(os.path.split(path)[0], pfx
                                    + os.path.split(path)[1])
                srcpath = os.path.join(self.miniatroot, path)
                destpath = os.path.join(self.rootdir, fname)

                srcpath = srcpath + sfx
                destpath = destpath + sfx

                srcpath = os.path.normpath(srcpath)
                destpath = os.path.normpath(destpath)

                parents = os.path.dirname(destpath)
                try:
                    os.makedirs(parents)
                except:
                    pass

                shutil.copyfile(srcpath, destpath)

                if name not in filetype:
                    filetype[name] = list()

                filetype[name].append(destpath)

        for t in filetype:

            if t == 'exe':
                self.outfile.write(Nsis.SCRIPT_INSTALL_SECTION_EXE)
            elif t == 'lib':
                self.outfile.write(Nsis.SCRIPT_INSTALL_SECTION_LIB)
            elif t == 'doc':
                self.outfile.write(Nsis.SCRIPT_INSTALL_SECTION_DOC)
            elif t == 'inc':
                self.outfile.write(Nsis.SCRIPT_INSTALL_SECTION_INC)
            elif t == 'src':
                pass
            else:
                raise Exception("Don't know what to do...")

            for f in filetype[t]:

                if t == 'src':
                    section = '    SetOutPath $INSTDIR\<BASE>\n' \
                        + '    SetOverwrite on\n'
                    base = os.path.split(f)[0]
                    base = os.path.normpath(base)
                    base = base.replace(self.rootdir, '')
                    if base[0] == os.path.sep:
                        base = base.replace(os.path.sep, '')

                    section = section.replace('<BASE>', base)
                    self.outfile.write(section)

                fline = str(Nsis.SCRIPT_FILE_LINE)
                fline = fline.replace('<FILE>', f)
                self.outfile.write('\t' + fline)

        self.outfile.write(Nsis.SCRIPT_FOOTER)
        self.outfile.close()

        origcwd = os.getcwd()

        os.chdir(self.rootdir)
        ps = subprocess.Popen(['makensis.exe', 'miniat.nsi'])
        ps.wait()
        os.chdir(origcwd)


def main(argv):

    pkglist = list()

    packageFile = open(PKG_PATH, 'r')
    prsr = xml.sax.make_parser()
    x = PackageParser(pkglist)
    prsr.setContentHandler(x)
    prsr.parse(packageFile)

    for pkg in pkglist:

        name = pkg.getName()
        nsis = Nsis(ROOT_PATH, name)
        items = pkg.getPkg()
        nsis.package(items)


if __name__ == '__main__':
    main(sys.argv)