source ../../common/bash/utils.sh.inc

function go
{
    tar=`basename $1`
    dir=`basename $1 .tar.gz`
    
    shift
 
    header $dir
    
    test -f $tar || wget $1
    test -d $dir || tar xzf $tar
    
    pushd $dir
    test -f config.h || ./configure $@
    perl -pi -e 's/-O3/-Zi -Oy -Ox -GL/g' Makefile
    make
    popd
}

function co
{
    svn co $1 $2
    pushd $2
    ./configure
    make
    popd
}


################################################################################

case `uname`
CYGWIN_NT_5.1)
    which make || die "You need to install GNU make"
    which ld || die "You need to install cygwin binutils"
    which wget || die "You need to install wget"

    go http://surfnet.dl.sourceforge.net/sourceforge/mad/libmad-0.15.1b.tar.gz --disable-debug
    go http://www.fftw.org/fftw-3.1.3.tar.gz --enable-float --disable-debug
    go http://www.mega-nerd.com/SRC/libsamplerate-0.1.4.tar.gz --disable-debug;;

Darwin)
    ;;
esac

#co http://google-breakpad.googlecode.com/svn/trunk/ breakpad


header Done!
echo "Now do make install in each directory or something"
echo