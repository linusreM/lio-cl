#pragma once

#include "stdint.h"

#define LIO_AUDIO_FSCALE 100

#define LIO_NOTE_C     1635
#define LIO_NOTE_CS    1732
#define LIO_NOTE_D     1835
#define LIO_NOTE_DS    1945
#define LIO_NOTE_E     2060
#define LIO_NOTE_F     2183
#define LIO_NOTE_FS    2312
#define LIO_NOTE_G     2450
#define LIO_NOTE_GS    2596
#define LIO_NOTE_A     2750
#define LIO_NOTE_AS    2914
#define LIO_NOTE_B     3087

void lio_start_audio_stream(void(*audio_handler)(uint32_t, uint16_t*));

void lio_stop_audio_stream();


void DMA1_Channel2_IRQHandler();


int32_t lio_get_frequency_sample(int32_t t, int32_t f);

void lio_output_sample(int32_t s);

void lio_init_audio(int32_t sample_rate);


