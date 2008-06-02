# we step into src because qmake is b0rked
cd src
qmake -recursive -tp vc moose.pro
