#!/bin/sh

find _bin -type f -perm +1 -name test_\* | xargs ./logTest.sh
