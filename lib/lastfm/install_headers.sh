if [ -z $1 ]
then
    echo "You must specify the prefix, eg. /usr"
    exit 0
fi

for x in `../../admin/findsrc pro`; do ../../admin/penis $x --cp; done
mkdir -p $1/include
cp -R _include/lastfm $1/include
cp global.h $1/include/lastfm
