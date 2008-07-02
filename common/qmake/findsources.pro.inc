defineReplace( findSources ) {
	return $$system( find . -name \*$$1 | sed -e 's|\./||g' | grep -v tests | grep -v _build )
}
