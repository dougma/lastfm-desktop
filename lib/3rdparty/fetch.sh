echo "This script uses wget, and the build chain."
echo "You'll need to do the make install step yourself in each directory."
echo

source ../../common/bash/utils.sh.inc

function go
{
    tar=`basename $1`
    dir=`basename $1 .tar.gz`
 
    header $dir
    
    test -f $tar || wget $1
    test -d $dir || tar xzf $tar
    
    pushd $dir
    ./configure
    make
    popd &>/dev/null
}

go http://surfnet.dl.sourceforge.net/sourceforge/mad/libmad-0.15.1b.tar.gz
go http://www.fftw.org/fftw-3.1.3.tar.gz
go http://www.mega-nerd.com/SRC/libsamplerate-0.1.4.tar.gz
