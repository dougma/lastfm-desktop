defineReplace( findSources ) {
    return( $$system( perl $$ROOT_DIR/common/qmake/findSources.pl $$1 ) )
}
