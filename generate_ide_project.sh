#!/bin/bash

BASE='Last.fm'

windows()
{
    qmake -recursive -tp vc $BASE.pro

    AFTER='
Project("{2150E333-8FDC-42A3-9474-1A3956D46DE8}") = "Solution Items", "Solution Items", "{780C13D3-8172-4EE0-8FD5-26ED6489851C}"
	ProjectSection(SolutionItems) = preProject
		include.pro = include.pro
		.qmake.cache = .qmake.cache
		Last.fm.pro = Last.fm.pro
		app\client\appclient.pro = app\client\appclient.pro
		lib\radio\libradio.pro = lib\radio\libradio.pro
		lib\moose\libmoose.pro = lib\moose\libmoose.pro
		lib\unicorn\libunicorn.pro = lib\unicorn\libunicorn.pro
		lib\fingerprint\libfingerprint.pro = lib\fingerprint\libfingerprint.pro
	EndProjectSection
EndProject
Project("{2150E333-8FDC-42A3-9474-1A3956D46DE8}") = "common", "common", "{3FDD67B7-DF67-4F22-8545-755D722794BC}"
	ProjectSection(SolutionItems) = preProject
		common\DllExportMacro.h = common\DllExportMacro.h
	EndProjectSection
EndProject
'

    echo "$AFTER" >> Last.sln
}

osx() 
{
    qmake -recursive -spec macx-xcode $BASE.pro
    # qmake 4.4 sucks
    for x in `find . -type d -name \*.xcodeproj`
    do 
        pushd $x &> /dev/null
        test -f project.pbxproj.* && mv project.pbxproj.* project.pbxproj
        popd &> /dev/null
    done
}

linux()
{
    echo unimplemented
}

# we step into src because qmake is b0rked
cd src

case `uname` in
    Darwin) osx;;
    Linux) linux;;
    *) windows;;
esac

rmdir _build
