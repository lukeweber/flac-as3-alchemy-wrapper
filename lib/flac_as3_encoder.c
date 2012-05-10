#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <FLAC/stream_encoder.h>
#include "config.h"
#include "AS3.h"

int resetPositionByteArray(AS3_Val byteArray)
{
	AS3_Val zero = AS3_Int(0);
	/* just reset the position */
	AS3_SetS((AS3_Val)byteArray, "position", zero);
	AS3_Release(zero);
	return 0;
}

FLAC__StreamEncoderReadStatus readByteArray(const FLAC__StreamEncoder* encoder, const FLAC__byte dataBuffer[], size_t bytes, void *writeBuf)
{
	int bytesRead = 0;
	bytesRead = AS3_ByteArray_readBytes((char *)dataBuffer, (AS3_Val)writeBuf, bytes);
	if(bytesRead == 0){
		return FLAC__STREAM_ENCODER_READ_STATUS_END_OF_STREAM;
	} else {
		return FLAC__STREAM_ENCODER_READ_STATUS_CONTINUE;
	}
}

FLAC__StreamEncoderWriteStatus writeByteArray(const FLAC__StreamEncoder* encoder, const FLAC__byte dataBuffer[], size_t bytes, unsigned samples, unsigned current_frame, void *writeBuf)
{
	fprintf(stderr, "bytes completed, %i, frame %i, samples %i, bufferdata %i", bytes, current_frame, samples, dataBuffer[0]);
	AS3_ByteArray_writeBytes((AS3_Val)writeBuf, (char *)dataBuffer, bytes);
	return FLAC__STREAM_ENCODER_WRITE_STATUS_OK;
}

//tell
FLAC__StreamEncoderTellStatus tellByteArray(const FLAC__StreamEncoder *encoder, FLAC__uint64 *absolute_byte_offset, void *writeBuf)
{
	AS3_Val as3position = AS3_GetS((AS3_Val)writeBuf, "position");
	int position = AS3_IntValue(as3position);
	AS3_Release(as3position);
	fprintf(stderr, "tell position %i", position);
	if((int) position < 0)
		return FLAC__STREAM_ENCODER_TELL_STATUS_ERROR;
	else {
		*absolute_byte_offset = (FLAC__uint64)position;
		return FLAC__STREAM_ENCODER_TELL_STATUS_OK;
	}
}

//seek
FLAC__StreamEncoderSeekStatus seekByteArray(const FLAC__StreamEncoder *encoder, FLAC__uint64 absolute_byte_offset, void *writeBuf)
{
	fprintf(stderr, "seek position %i", (int)absolute_byte_offset);
	AS3_ByteArray_seek((AS3_Val)writeBuf, (int)absolute_byte_offset, SEEK_SET);
	//return FLAC__STREAM_ENCODER_SEEK_STATUS_ERROR;
	return FLAC__STREAM_ENCODER_SEEK_STATUS_OK;
}

static AS3_Val encodeForFlash(void * self, AS3_Val args)
{
	AS3_Val progress;
	AS3_Val src, dest;
	FLAC__StreamEncoder* encoder;
	int srcLen, remainingBytes, bytesRead, yieldTicks, encodeAsOgg;
	FLAC__int32 raw_data[1024];//32 bit, 1024 * 4
	FLAC__int32 *channels_array[1];
	short raw_buffer[1024];
	channels_array[0] = raw_data;

	AS3_ArrayValue(args, "AS3ValType, AS3ValType, AS3ValType, IntType, IntType, IntType", &progress, &src, &dest, &srcLen, &yieldTicks, &encodeAsOgg);

	encoder = FLAC__stream_encoder_new();
	FLAC__stream_encoder_set_channels(encoder, 1);
	FLAC__stream_encoder_set_bits_per_sample(encoder, 16);
	FLAC__stream_encoder_set_sample_rate(encoder, 16000);
	FLAC__stream_encoder_set_total_samples_estimate(encoder, srcLen / sizeof(short));
	if(encodeAsOgg){
		FLAC__stream_encoder_init_ogg_stream(encoder, readByteArray, writeByteArray, seekByteArray, tellByteArray, NULL/*metadata callback*/, dest);
	} else {
		FLAC__stream_encoder_init_stream(encoder, writeByteArray, seekByteArray, tellByteArray, NULL/*metadata callback*/, dest);
	}

	int i = 0;
	bytesRead = 0;
	remainingBytes = srcLen;
	resetPositionByteArray(src);
	while (remainingBytes > 0){
		bytesRead = AS3_ByteArray_readBytes((void *)raw_buffer, src, 1024 * sizeof(short));
		int j;
		for(j = 0; j < bytesRead / sizeof(short); j++){
			raw_data[j] = (FLAC__int32) raw_buffer[j];
		}
		remainingBytes -= bytesRead;
		fprintf(stderr, "bytes read from src %i", bytesRead);
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
	return AS3_Int(1);
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
