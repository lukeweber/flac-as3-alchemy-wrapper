Flac Alchemy Encoder
=============

[Free Lossless Audio Codec](http://flac.sourceforge.net/index.html) (FLAC). This is a SWC that wraps the base functionality of the FLAC codec.

### Building FLAC (optional)

### Compile iLBC.swc (optional)
[Download Adobe Alchemy](http://labs.adobe.com/downloads/alchemy.html)

[Download flex 4.6](http://opensource.adobe.com/wiki/display/flexsdk/Download+Flex+4.6)

[Getting Started/Install Adobe Alchemy](http://labs.adobe.com/wiki/index.php/Alchemy:Documentation:Getting_Started)

After having installed Alchemy and Flex SDK, just run the commands below:

	$ alc-on
	$ make all

Example
------------
Full Usage of flac.swc at src/net/sourceforge/flac/codec/FLACCodec.as

### AS3 Code

Methods encode and decode are asyncronous and take as their first params a callback for completed. As well yield param is frequency of loops of processing audio that will be completed before the function calls flyield(). flyeidl(): "This method will force the Alchemy state machine to give up it's time slice. Execution will return to the line following this call on the next Alchemy time-slicing timer tick." - Time slicing timer tick I believe is 1ms. 

Imports

	import cmodule.flac.CLibInit;
	import org.ilbc.event.FLACEvent;

Initialization of lib

	private var flacCodec:Object;
	ilbcCodec = (new cmodule.flac.CLibInit).init();

Encode

	encodedData = new ByteArray();
	encodedData.endian = Endian.LITTLE_ENDIAN;
	flacCodec.encode(encodingCompleteHandler, encodingProgressHandler, rawPCMByteArray, encodedData, decodedData.length, yield);

Progress Handler

	function progressHandler(progress:int):void;