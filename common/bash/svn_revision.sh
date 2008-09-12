#!!/bin/bash
svn info | grep "Last Changed Rev" | cut -d' ' -f4