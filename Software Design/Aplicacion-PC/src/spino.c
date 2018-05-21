/* MIT License
 *
 * Copyright (c) 2017 spino.tech Guillaume Chereau & Raphael Seghier
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#include "spino.h"
#include "lz4hc.h"
//#include <string.h>
static int spino_compress(const uint8_t *src, int src_size,
                          const uint8_t *prev, int prev_size,
                          uint8_t *out, int out_max_size)
{
	int i;
	FILE *f;
	f=fopen("#descomprimido.txt","w");
	
    int ret;
    LZ4_streamHC_t *stream = NULL;
    stream = LZ4_createStreamHC();
    LZ4_resetStreamHC(stream, 16);
    LZ4_loadDictHC(stream, (const char*)prev, prev_size);
    fprintf(f,"| ");
    for (i=0;i<src_size;i++){
	if(((int) src[i]) < 10){
		fprintf(f," 0%d | ",(int) src[i]);
	}else
	{
	fprintf(f," %d | ",(int) src[i]);
	}
	}
	fprintf(f," \n-------------------------------------- \n");
	
    ret = LZ4_compress_HC_continue(stream, (const char*)src,
                                   (char*)out, src_size, out_max_size);
                                   
    LZ4_freeStreamHC(stream);
    fclose(f);
    return ret;
}

static void write_uint32(uint8_t *dst, uint32_t v)
{
    dst[0] = (v & 0x000000ff) >> 0;
    dst[1] = (v & 0x0000ff00) >> 8;
    dst[2] = (v & 0x00ff0000) >> 16;
    dst[3] = (v & 0xff000000) >> 24;
}

int spino_make_frame(const uint8_t *data, int size,
                     const uint8_t *prev_data, int prev_data_size,
                     bool indep, bool line_mode, int pwm,
                     uint8_t *out, int out_max_size)
{
    // 10 bytes header followed by compressed data
    //
    //  0 1 2 3 4 5 6 7 8 9
    // +-------+-+-+-------+---------------------------------------------+
    // |T D 2 - G 8|Size   | data                                        |
    // +-------+-+-+-------+---------------------------------------------+
    int comp_size;
    const int h = SPINO_HEADER_SIZE;

    // Compress the data first so that we get the compressed size.
    comp_size = spino_compress(data, size, prev_data, prev_data_size,
                               out + h, out_max_size - h);

    memcpy(out, "TD2-G8", 6);
    write_uint32(&out[6], comp_size);
    return comp_size + h;
}
