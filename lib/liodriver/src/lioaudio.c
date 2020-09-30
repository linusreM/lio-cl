#include "lioaudio.h"
#include "stdint.h"
#include "gd32vf103.h"

#define LIO_AUDIO_TABLE_SIZE    256
#define LIO_DAC_OUTPUT_BIAS     2048

int32_t audio_base_frequency_g = 22*1024;

#define CLAMP(x, low, high)  (((x) > (high)) ? (high) : (((x) < (low)) ? (low) : (x)))


static void audio_dma_config(void);


static const uint16_t sin_table[LIO_AUDIO_TABLE_SIZE] = {
    0,     402,   804,   1206,  1608,  2010,  2412,  2813, 
    3215,  3617,  4018,  4419,  4820,  5221,  5622,  6023, 
    6423,  6823,  7223,  7622,  8022,  8420,  8819,  9217, 
    9615,  10013, 10410, 10807, 11203, 11599, 11995, 12390, 
    12785, 13179, 13572, 13965, 14358, 14750, 15142, 15533, 
    15923, 16313, 16702, 17090, 17478, 17865, 18252, 18638, 
    19023, 19407, 19791, 20174, 20556, 20938, 21318, 21698, 
    22077, 22455, 22833, 23209, 23585, 23960, 24333, 24706, 
    25078, 25449, 25819, 26188, 26557, 26924, 27290, 27655, 
    28019, 28382, 28744, 29105, 29464, 29823, 30180, 30537, 
    30892, 31246, 31599, 31951, 32301, 32650, 32998, 33345, 
    33691, 34035, 34378, 34720, 35060, 35399, 35737, 36073, 
    36408, 36742, 37074, 37405, 37735, 38063, 38389, 38714, 
    39038, 39360, 39681, 40000, 40318, 40634, 40949, 41262, 
    41574, 41884, 42192, 42499, 42805, 43108, 43410, 43711, 
    44009, 44307, 44602, 44896, 45188, 45478, 45767, 46054, 
    46339, 46622, 46904, 47184, 47462, 47739, 48013, 48286, 
    48557, 48826, 49093, 49359, 49622, 49884, 50144, 50402, 
    50658, 50912, 51164, 51415, 51663, 51909, 52154, 52396, 
    52637, 52875, 53112, 53347, 53579, 53810, 54038, 54265, 
    54489, 54711, 54932, 55150, 55366, 55580, 55792, 56002, 
    56210, 56416, 56619, 56821, 57020, 57217, 57412, 57605, 
    57795, 57984, 58170, 58354, 58536, 58716, 58893, 59068, 
    59242, 59412, 59581, 59747, 59911, 60073, 60233, 60390, 
    60545, 60698, 60848, 60996, 61142, 61286, 61427, 61566, 
    61703, 61837, 61969, 62099, 62226, 62351, 62473, 62594, 
    62712, 62827, 62940, 63051, 63160, 63266, 63369, 63471, 
    63570, 63666, 63760, 63852, 63941, 64028, 64113, 64195, 
    64274, 64352, 64426, 64499, 64569, 64636, 64701, 64764, 
    64824, 64882, 64937, 64990, 65041, 65089, 65134, 65177, 
    65218, 65256, 65292, 65325, 65356, 65384, 65410, 65434, 
    65455, 65473, 65489, 65503, 65514, 65522, 65529, 65532
};
#define AUDIO_DMA_BUFFER_SIZE 256
uint16_t audio_dma_buffer[AUDIO_DMA_BUFFER_SIZE];
dma_parameter_struct dma_struct;
static uint32_t buffer_toggle;

static void (*buffer_handler)(uint32_t samples, uint16_t *buffer) = NULL;


void lio_start_audio_stream(void(*audio_handler)(uint32_t, uint16_t*)){
    //Setup callback function
    buffer_handler = audio_handler;
    //Fill buffer first time (both buffers)
    audio_handler(AUDIO_DMA_BUFFER_SIZE, &audio_dma_buffer[0]);
    //Start dma with IRQ
    audio_dma_config();
}

void lio_stop_audio_stream(){
    dma_interrupt_disable(DMA1, DMA_CH2, DMA_INT_FTF);
    dac_dma_disable(DAC0);
}


void DMA1_Channel2_IRQHandler()
{
    if(dma_interrupt_flag_get(DMA1, DMA_CH2, DMA_INT_FLAG_FTF)){

        dma_interrupt_flag_clear(DMA1, DMA_CH2, DMA_INT_FLAG_G);
    
        dma_memory_address_config(DMA1, DMA_CH2,  (int32_t)&audio_dma_buffer[buffer_toggle * (AUDIO_DMA_BUFFER_SIZE / 2)]);
        dac_dma_enable(DAC0);
        buffer_toggle++;
        buffer_toggle %= 2;

        buffer_handler(AUDIO_DMA_BUFFER_SIZE/2, (uint16_t*)&audio_dma_buffer[buffer_toggle * (AUDIO_DMA_BUFFER_SIZE / 2)]);
    }


}


static void audio_dma_config(void)
{
    rcu_periph_clock_enable(RCU_DMA1);
    
    rcu_periph_clock_enable(RCU_TIMER5);
    
    dma_interrupt_flag_clear(DMA1, DMA_CH2, DMA_INT_FLAG_G);
    /* clear all the interrupt flags */
    dma_flag_clear(DMA1, DMA_CH2, DMA_INTF_GIF);
    dma_flag_clear(DMA1, DMA_CH2, DMA_INTF_FTFIF);
    dma_flag_clear(DMA1, DMA_CH2, DMA_INTF_HTFIF);
    dma_flag_clear(DMA1, DMA_CH2, DMA_INTF_ERRIF);
    
    /* configure the DMA1 channel 2 */
    dma_struct.periph_addr  = (0x4000740C);
    dma_struct.periph_width = DMA_PERIPHERAL_WIDTH_32BIT;
    dma_struct.memory_addr  = (uint32_t)audio_dma_buffer;
    dma_struct.memory_width = DMA_MEMORY_WIDTH_16BIT;
    dma_struct.number       = AUDIO_DMA_BUFFER_SIZE/2;
    dma_struct.priority     = DMA_PRIORITY_ULTRA_HIGH;
    dma_struct.periph_inc   = DMA_PERIPH_INCREASE_DISABLE;
    dma_struct.memory_inc   = DMA_MEMORY_INCREASE_ENABLE;
    dma_struct.direction    = DMA_MEMORY_TO_PERIPHERAL;
    
    dma_init(DMA1, DMA_CH2, &dma_struct);

    dma_circulation_enable(DMA1, DMA_CH2);
    dma_interrupt_enable(DMA1, DMA_CH2, DMA_INT_FTF);

    /* configure the TIMER5 */
    timer_prescaler_config(TIMER5, 0x0, TIMER_PSC_RELOAD_UPDATE);
    timer_autoreload_value_config(TIMER5, SystemCoreClock/audio_base_frequency_g);
    timer_master_output_trigger_source_select(TIMER5, TIMER_TRI_OUT_SRC_UPDATE);
    timer_enable(TIMER5);


    eclic_global_interrupt_enable();
    eclic_priority_group_set(ECLIC_PRIGROUP_LEVEL3_PRIO1);
    eclic_irq_enable(DMA1_Channel2_IRQn, 1, 1);
    dma_channel_enable(DMA1, DMA_CH2);
    dac_dma_enable(DAC0);
}

int32_t sin_12bit(int32_t index)
{
    index %= (LIO_AUDIO_TABLE_SIZE*4);
    index = index < 0 ? (LIO_AUDIO_TABLE_SIZE*4) - index : index; 
    uint32_t invert = (index >> 8)%2;
    uint32_t sign = (index >> 9);
    index %= LIO_AUDIO_TABLE_SIZE;
    index = invert ? LIO_AUDIO_TABLE_SIZE - index - 1 : index;
    int32_t ret = sin_table[index] >> 5;
    return sign ? -ret : ret;
}


int32_t lio_get_frequency_sample(int32_t t, int32_t f){
    return sin_12bit(t*((f*LIO_AUDIO_TABLE_SIZE*4)/audio_base_frequency_g));
}

void lio_output_sample(int32_t s){
    s += LIO_DAC_OUTPUT_BIAS;
    s = CLAMP(s, 0, 4095);
    dac_data_set(DAC0, DAC_ALIGN_12B_L, s);
}

void lio_init_audio(int32_t sample_rate){
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_DAC);
    
    gpio_init(GPIOA, GPIO_MODE_AIN, GPIO_OSPEED_50MHZ, GPIO_PIN_4);

    dac_deinit();
    /* configure the DAC0 */
    dac_trigger_source_config(DAC0, DAC_TRIGGER_T5_TRGO);
    
    dac_trigger_enable(DAC0);
    dac_wave_mode_config(DAC0, DAC_WAVE_DISABLE);
    dac_output_buffer_enable(DAC0);
    
    dac_enable(DAC0);

    dac_dma_enable(DAC0);
}

//22000 / 1024 ~= 22Hz