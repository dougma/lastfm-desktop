"%WIX%"\bin\candle Product.wxs
"%WIX%"\bin\light -ext WixUIExtension -cultures:en-us Product.wixobj -b ..\_bin\ -b C:\dev\Qt\4.4.3\bin