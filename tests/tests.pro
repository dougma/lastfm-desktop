TEMPLATE = subdirs
SUBDIRS += $$system( ls ../app/client/tests/*.pro )
SUBDIRS += $$system( ls ../lib/moose/tests/*.pro )
