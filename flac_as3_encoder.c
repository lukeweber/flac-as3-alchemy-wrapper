#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <FLAC/stream_encoder.h>
#include "AS3.h"

int resetPositionByteArray(AS3_Val byteArray)
{
	AS3_Val zero = AS3_Int(0);
	/* just reset the position */
	AS3_SetS((AS3_Val)byteArray, "position", zero);
	AS3_Release(zero);
	return 0;
}

FLAC__StreamEncoderWriteStatus writeByteArray(void *encoder, void *writeBuf, size_t size, unsigned samples, unsigned current_frame, void *src)
{
	AS3_ByteArray_writeBytes((AS3_Val)writeBuf, (char *)src, size * samples);
	return FLAC__STREAM_ENCODER_WRITE_STATUS_OK;
}

static void encodeForFlash(void * self, AS3_Val args)
{
	AS3_Val progress;
	AS3_Val src, dest;
	FLAC__StreamEncoder* encoder;
	int srcLen, remainingBytes, yieldTicks;
	FLAC__int32 raw_data[1024];
	FLAC__int32 *channels_array[1];
	channels_array[0] = raw_data;

	AS3_ArrayValue(args, "AS3ValType, AS3ValType, AS3ValType, IntType, IntType", &progress, &src, &dest, &srcLen, &yieldTicks);
	
	encoder = FLAC__stream_encoder_new();
	FLAC__stream_encoder_set_channels(encoder, 1);
	FLAC__stream_encoder_set_bits_per_sample(encoder, 16);
	FLAC__stream_encoder_set_sample_rate(encoder, 16000);
	FLAC__stream_encoder_set_total_samples_estimate(encoder, srcLen / sizeof(short));
	FLAC__stream_encoder_init_stream(encoder, (FLAC__StreamEncoderWriteCallback)writeByteArray, NULL, NULL, NULL, dest);

	int i = 0;
	int bytesRead = 0;
	resetPositionByteArray(src);
	while (remainingBytes > 0){
		bytesRead = AS3_ByteArray_readBytes((void *)raw_data, src, 512 * sizeof(short));
		remainingBytes -= bytesRead;
		FLAC__stream_encoder_process(encoder, (const FLAC__int32 *const *)channels_array, bytesRead / sizeof(short));
		if(i % yieldTicks == 0){
			AS3_CallT(progress, NULL, "IntType", (int)((1 - ((float)remainingBytes / srcLen)) * 100));
			flyield();//yield to main process
		}
		i++;
	}

	resetPositionByteArray(src);
	resetPositionByteArray(dest);
	FLAC__stream_encoder_finish(encoder);
	FLAC__stream_encoder_delete(encoder);
	// Don't remove progess 100 call here, else complete won't be called!
	AS3_CallT(progress, NULL, "IntType", 100);
}

int main(int argc, char **argv)
{
	AS3_Val flac_lib = AS3_Object("");
	AS3_Val encodeMethod = AS3_FunctionAsync( NULL, (AS3_ThunkProc) encodeForFlash);
	AS3_SetS(flac_lib, "encode", encodeMethod);
	AS3_Release( encodeMethod );

	//No code below here. AS3_LibInit does not return
	AS3_LibInit( flac_lib );
	return 0;
}
