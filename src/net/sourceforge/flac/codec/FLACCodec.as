package net.sourceforge.flac.codec {
	
	import cmodule.flac.CLibInit;

	import net.sourceforge.flac.event.FLACEvent;

	import flash.events.EventDispatcher;
	import flash.events.ProgressEvent;
	import flash.utils.ByteArray;
	import flash.utils.Endian;
	import flash.utils.getTimer;
	
	
	[Event(name="FLACEventEncodingComplete", type="net.sourceforge.flac.event.FLACEvent")]
	[Event(name="FLACEventDecodingComplete", type="net.sourceforge.flac.event.FLACEvent")]
	[Event(name="progress", type="flash.events.ProgressEvent")]
	
	/**
	 * Wrapper class for the C FLAC codec.
	 * 
	 * @author Wijnand Warren
	 */
	public class FLACCodec extends EventDispatcher {
		
		public var encodedData:ByteArray;
		public var decodedData:ByteArray;
		
		private var flacCodec:Object;
		private var initTime:int;
		
		/**
		 * CONSTRUCTOR
		 */
		public function FLACCodec() {
			init();
		}
		
		/**
		 * Initializes this class.
		 */
		private function init():void {
			initTime = -1;
		}
		
		// ------------------------
		// PRIVATE METHODS
		// ------------------------
		
		/**
		 * 
		 */
		private function start() : void {
			initTime = getTimer();
			flacCodec = (new cmodule.flac.CLibInit).init();
		}
		
		/**
		 * Calculates how long an encoding or decoding process took.
		 */
		private function getElapsedTime():int {
			var now:int = getTimer();
			return now - initTime;
		}
		
		// ------------------------
		// EVENT HANDLERS
		// ------------------------
		
		/**
		 * Called when encoding has finished.
		 */
		private function encodingCompleteHandler(event:*):void {
			trace("FLACCodec.encodingCompleteHandler(event):", event);

			encodedData.position = 0;
			dispatchEvent( new FLACEvent(FLACEvent.ENCODING_COMPLETE, encodedData, getElapsedTime()) );
		}
		
		/**
		 * Called when the encoding task notifies progress.
		 */
		private function encodingProgressHandler(progress:int):void {
			trace("FLACCodec.encodingProgressHandler(event):", progress);
			dispatchEvent( new ProgressEvent(ProgressEvent.PROGRESS, false, false, progress, 100));
		}

		/**
		 * Called when decoding has finished.
		 */
		private function decodingCompleteHandler(event:*):void {
			trace("FLACCodec.decodingCompleteHandler(event):", event);
			
			decodedData.position = 0;
			dispatchEvent( new FLACEvent(FLACEvent.DECODING_COMPLETE, decodedData, getElapsedTime()) );
		}

		
		/**
		 * Called when the decoding task notifies progress.
		 */
		private function decodingProgressHandler(progress:int):void {
			trace("FLACCodec.decodingProgressHandler(event):", progress);
			dispatchEvent( new ProgressEvent(ProgressEvent.PROGRESS, false, false, progress, 100));
		}
		
		// ------------------------
		// PUBLIC METHODS
		// ------------------------
		
		/**
		 * Encodes an (16 bit 16kHz) audio stream to FLAC.
		 */
		public function encode(data:ByteArray):void {
			trace("FLACCodec.encode(data):", data.length);
			
			encodedData = new ByteArray();
			encodedData.endian = Endian.LITTLE_ENDIAN;
			decodedData = data;
			
			start();
			
			flacCodec.encode(encodingCompleteHandler, encodingProgressHandler, decodedData, encodedData, decodedData.length, 10);
		}
		
		/**
		 * Decodes a FLAC encoded audio stream (16 bit 16kHz).
		 */
		public function decode(data:ByteArray):void {
			trace("FLACCodec.decode(data):", data.length);
			
			encodedData = data;
			decodedData = new ByteArray();
			decodedData.endian = Endian.LITTLE_ENDIAN;
			
			start();
			
			flacCodec.decode(decodingCompleteHandler, decodingProgressHandler, encodedData, decodedData, encodedData.length, 10);
		}
		
	}
	
}
