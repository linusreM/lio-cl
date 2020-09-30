#include "FreeRTOS.h" /* Must come first. */
#include "task.h"     /* RTOS task related API prototypes. */
#include "queue.h"    /* RTOS queue related API prototypes. */
#include "timers.h"   /* Software timer related API prototypes. */
#include "semphr.h"   /* Semaphore related API prototypes. */

#include "gd32vf103.h"
#include "systick.h"

#include "cltool.h"
#include "usb_serial_if.h"
#include "liomatrix.h"
#include "liomotor.h"
#include "liobt.h"
#include "lcd.h"
#include "string.h"
#include "float.h"
#include "stdlib.h"
#include "math.h"
#include "lioaudio.h"

#include "command_data.h"




#define MOTOR_SPEED 1023


const char version_string[] = "Firmware version 0.1 -- 29/9/2020 (FreeRTOS variant)\r\n\r";

int print_help(int argc, char *argv[]);

//Placeholder function for unimplemented items in command list
int placeholder(int argc, char *argv[])
{
    printf("Placeholder\r\n\r");
    return 0;
}

int print_version(int argc, char *argv[])
{
    printf(version_string);
    return 1;
}

int print_arg(int argc, char *argv[])
{   
    printf("\r\n\r");
    for(int i = 0; i < argc; i++) printf("Arg %d: %s \r\n\r", i, argv[i]);
    return 1;
}

int set_matrix_num(int argc, char *argv[])
{
    if(argc > 1)lio_write_led_data(matrix_data[atoi(argv[1])]);
    return 0;
}

int set_matrix_pattern(int argc, char *argv[])
{
    lio_row_data_t led_data = {{0,0,0,0,0,0,0}};

    for(int i = 1; i < argc; i++) led_data.row[i-1] = strtoul(argv[i], NULL, 16);

    lio_write_led_data(led_data);

    return 0;
}

int set_motor_dutycycle(int argc, char *argv[])
{
    if(argc < 3) return 0;
    if(argv[1][0] == 'A') lio_motor_speed(LIO_MOTOR_A, atoi(argv[2]));
    if(argv[1][0] == 'B') lio_motor_speed(LIO_MOTOR_B, atoi(argv[2]));
    return 1;
}

int init_motor(int argc, char *argv[]){
    int f, res;
    if(argc < 3) f = 10000; 
    else f = atoi(argv[2]);

    if(argc < 4) res = 10;
    else res = atoi(argv[3]);

    if(argv[1][0] == 'A') lio_motor_init(LIO_MOTOR_A, LIO_MOTOR_NORMAL, f, res);
    if(argv[1][0] == 'B') lio_motor_init(LIO_MOTOR_B, LIO_MOTOR_INVERT, f, res);

    return 1;
}

int init_led_matrix(int argc, char *argv[])
{
    int f;
    if(argc < 2) f = 1000;
    else f = atoi(argv[1]);

    lio_init_led_matrix(f);
    return 1;
}

int init_keypad(int argc, char *argv[])
{
    int db;
    if(argc < 2) db = 100;
    else db = atoi(argv[1]);
    
    lio_init_keypad(db);
    return 1;
}

int get_key(int argc, char *argv[])
{
    //Discard prior presses
    lio_key_value_t key_pressed = lio_read_keypad().value;

    key_pressed = lio_read_keypad().value;

    if(argc == 1) while(key_pressed == LIO_KEY_NONE){
        vTaskDelay(2);
        key_pressed = lio_read_keypad().value;
    } 
    else while(!(key_pressed == strtoul(argv[1], NULL, 16))) {
        key_pressed = lio_read_keypad().value;
        vTaskDelay(2);
        }
    printf("Key pressed: %d\r\n\r", key_pressed);
    return 1;
}

int print_memory_region(int argc, char *argv[])
{
    if(argc != 3) return 0;
    uint8_t *sp = (uint8_t*)strtoul(argv[1], NULL, 16);
    uint8_t *ep = (uint8_t*)strtoul(argv[2], NULL, 16);
    int count = 0;
    //printf("%.8x: ", sp);
    while(sp <= ep){
        if(!(count++ % 8)) printf("\r\n\r%.8x: ", (unsigned int)sp);
        printf("%.2x ", *sp);
        if(!(count % 4)) printf("  ");
        sp++;
    }
    printf("\r\n\r");
    return 1;
}

int write_memory_address(int argc, char *argv[])
{
    if(argc != 3) return 0;
    uint32_t *ap = (uint32_t*) strtoul(argv[1], NULL, 16);
    *ap = strtoul(argv[2], NULL, 16);
    return 1;

}

int lcd_init(int argc, char *argv[])
{
    Lcd_Init();
    LCD_Clear(BLACK);
    return 1;
}

int lcd_rect(int argc, char *argv[])
{
    if(argc != 6) return 0;
    uint16_t color = BLACK;
    switch(argv[1][0])
    {
        case 'r': color = RED; break;
        case 'g': color = GREEN; break;
        case 'b': color = BLUE; break;
        case 'y': color = YELLOW; break;
        case 'c': color = CYAN; break;
        case 'm': color = MAGENTA; break;
        case 'k': color = BLACK; break;
    }
    LCD_Fill(atoi(argv[2]), atoi(argv[3]), atoi(argv[4]), atoi(argv[5]), color);

    return 1;
}

int lcd_txt(int argc, char *argv[])
{
    int x = atoi(argv[1]);
    int y = atoi(argv[2]);
    for(int i = 3; i < argc; i++)
    {
        if(x + (strlen(argv[i]) * 8) > 160){
            x = 0;
            y += 16;
        }
        LCD_ShowStr(x, y, (uint8_t*)argv[i], WHITE, TRANSPARENT);
        x += strlen(argv[i]) * 10;
    }
    return 1;
}


int lcd_triangle(int argc, char *argv[])
{
    if(argc != 8) return 0;
    int p12, p13, p23;
    uint16_t color = strtoul(argv[1], NULL, 16);

    int p1x = atoi(argv[2]);
    int p1y = atoi(argv[3]);
    int p2x = atoi(argv[4]);
    int p2y = atoi(argv[5]);
    int p3x = atoi(argv[6]);
    int p3y = atoi(argv[7]);

     /* Get bounding box */
    int minx = p1x;
    int maxx = p1x;
    int miny = p1y;
    int maxy = p1y;

    minx = minx < p2x ? minx : p2x;
    minx = minx < p3x ? minx : p3x;

    miny = miny < p2y ? miny : p2y;
    miny = miny < p3y ? miny : p3y;

    maxx = maxx > p2x ? maxx : p2x;
    maxx = maxx > p3x ? maxx : p3x;

    maxy = maxy > p2y ? maxy : p2y;
    maxy = maxy > p3y ? maxy : p3y;

    for(int y = miny < 0 ? 0 : miny; y <= maxy && y < 80; y++)
    {
        for(int x = minx < 0 ? 0 : minx; x <= maxx && x < 160; x++)
        {
            /* Is the current point inside the triangle? 
            Uses cross product to determine which side of the walls of the
            triangle the point is. */
            p12 = (p2x - p1x) * (y - p1y) - (p2y - p1y) * (x - p1x) > 0;
            p13 = (p3x - p1x) * (y - p1y) - (p3y - p1y) * (x - p1x) > 0;
            p23 = (p3x - p2x) * (y - p2y) - (p3y - p2y) * (x - p2x) > 0;

            if((p12 == p23) && (p13 != p12)) LCD_DrawPoint(x, y, color);    
        }
    }
    return 1;
}

int print_float(int argc, char *argv[])
{
    float f = atof(argv[1]);
    
    printf("Float:%f, Integer:%d, Integer*100:%d \r\n\r",f,(int)f, (int)(f*100.0));
    return 1;
}

int uart_term(int argc, char *argv[])
{
    uint8_t btbuffer[256] = {'\0'};
    uint8_t usbbuffer[64] = {'\0'};
    uint8_t exit_sequence[] = "exit!\0";
    uint8_t *p_exit = exit_sequence;
    int exit = 0;
    lio_init_bt();
    while(!exit)
    {
        if(read_usb_serial(usbbuffer))
        {
            if(usbbuffer[0] == *p_exit) p_exit++;
            else p_exit = exit_sequence;
            if(*p_exit == '\0') break;
            lio_send_bt((char*)usbbuffer, strlen((char*)usbbuffer));
            usbbuffer[0] = '\0';
        }
        if(lio_read_bt((char*)btbuffer, 256))printf("%s\n", btbuffer);
        vTaskDelay(10);
    }


    return 1;
}

int audio_init(int argc, char *argv[]){
    uint32_t sample_rate = 44*1024;
    if(argc > 1) sample_rate = atoi(argv[1]);
    lio_init_audio(sample_rate);
    return 0;
}

//This is a callback used in the audio-tone function
volatile uint32_t audio_time = 128;
int32_t audio_freq = 0;
void audio_handler_cb(uint32_t samples, uint16_t *buffer){
    for(int i = 0; i < samples; i++, audio_time++){
        /*You can mess around with the harmonics to make some interesting sounds, 
          the get frequency sample call isn't very expensive so you could get away with
          a lot of harmonics and subtones without hitting the computational limit*/
        buffer[i] =((lio_get_frequency_sample(audio_time, audio_freq  )/ 6) + 
                    (lio_get_frequency_sample(audio_time, audio_freq*2)/ 8) +       //First harmonic
                    (lio_get_frequency_sample(audio_time, audio_freq*4)/ 16) +      //Third harmonic
                    (lio_get_frequency_sample(audio_time, audio_freq-220)/32) +     //Some Low end
                    (lio_get_frequency_sample(audio_time, 100)/32) +                //Some 100Hz
                    (lio_get_frequency_sample(audio_time, audio_freq*3)/32))+2048;  //Second harmonic
    }
}

int audio_tone(int argc, char *argv[]){
    int time_ms = atoi(argv[1]);
    audio_freq = atoi(argv[2]);

    lio_start_audio_stream(&audio_handler_cb);
    
    //Wait for the duration
    vTaskDelay(time_ms/2);

    lio_stop_audio_stream();

    return 0;
}



/*To add a command to the command line tool just put the name, function and description in this table.
The function must be a - int fn(int argc, char *argv[]) - function. argc denotes how many arguments
are in the call. *argv[] contains an array of arguments in string form. The first argument is always
the command name.*/
cltool_command_vector_t vector_table[] = {

    //Utility functions
    {"help", &print_help, 
     "View available functions."},

    {"ver", &print_version, 
     "Prints version information."},

    {"print-mem", &print_memory_region,
     "[start addr] [end addr] print memory region"},

    {"write-mem", &write_memory_address,
     "[addr] [data] write a 32 bit value to memory"},

    //Commandline test functions
    {"print-arg", &print_arg,
    "Prints supplied arguments (test for cltool)"},

    {"print-float", &print_float,
      "Prints a parsed float."},

    //UART
    {"uart-term", &uart_term,
      "Prints a parsed float."},

    //LED/Key matrix functions
    {"led-init", &init_led_matrix,
    "[Rate] Initialize led display"},
    
    {"led-num", &set_matrix_num, 
     "[number] Set the led matrix pattern to a number"},

    {"led-pattern", &set_matrix_pattern,
     "[First row][second row] ... [eigth row] set the led matrix pattern"},

    {"key-init", &init_keypad,
    "Initialize keypad"},

    {"key-get", &get_key,
    "Get a keypress"},

    //Motor functionality
    {"mot-speed", &set_motor_dutycycle, 
     "[motor 'A' or 'B'] [dutycycle] Set motor pwm"},

    {"mot-init", &init_motor,
     "[motor 'A' or 'B'] <PWM frequency> <Resolution (bits)>"},

    //LCD
    {"lcd-init", &lcd_init, 
     "Initialize LCD."},
    
    {"lcd-rect", &lcd_rect, 
     "[color: r/g/b/c/m/y/k] [x] [y] [x2] [y2] Draws a rectangle."},

    {"lcd-txt", &lcd_txt, 
     "[x] [y] [Text to display] Puts some text on the LCD."},

    {"lcd-tri", &lcd_triangle, 
     "[x1] [y1] [x2] [y2] [x3] [y3] Draws a triangle to the screen."},

    //Audio
    {"audio-init", &audio_init,
    "Start the audio engine"},

    {"audio-tone", &audio_tone,
     "[time(ms)] [frequency] Plays a tone for the duration"},

    //TODO:
    // * Accellerometer
    // * SD-card
    // * 1-wire DS18B20S (still needs driver support)

    //Termination entry, do not remove or bad things will happen.
    {"\0", 0, "\0"}

};

int print_help(int argc, char *argv[]){
    cltool_command_vector_t *cmd = vector_table;

    printf("\r\n\r");
    vTaskDelay(2);
    printf("Usage: [Function] [arg1] [arg2] ... \r\n\r");
    vTaskDelay(2);
    printf("\r\n\r");
    if(argc <= 1)
    {
        vTaskDelay(2);
        while(cmd->name[0] != '\0')
        {
            printf("%s - %s\r\n\r", cmd->name, cmd->help);
            cmd++;
            vTaskDelay(2);
        }
    }
    else
    {
        while(cmd->name[0] != '\0')
        {
            if(!strncmp(cmd->name, argv[1], 32))
            {
                printf("%s\r\n\r", cmd->help);
                return 0;
            } 
            cmd++;
        }
    }
    return 1;
}


void start_task(void *pvParameters){
    lio_init_led_matrix(1000);
    lio_write_led_data(matrix_data[1]);
    int counter = 0;
    while(1){
        counter += 1;
        counter %= 10;
        vTaskDelay(500);
        lio_write_led_data(matrix_data[counter]);
    }
}


void main_task(void *pvParameters){
    uint8_t receive_buffer[64];
    cltool_config_t cltool;
    cltool.p_com_vector = vector_table;
    cltool.terminator = '\r';
    cltool_init(cltool);

    vTaskDelay(1500);
    printf("Please press 'k' in terminal or press a keypad button.\r\n\r");
    read_usb_serial(receive_buffer);

    

    while(receive_buffer[0] != 'k' && lio_read_keypad().value == LIO_KEY_NONE)
    { 
        read_usb_serial(receive_buffer);
        vTaskDelay(50);
    }

    printf("Welcome to the LAB-IO test suite.\r\n\r");
    
    while(1)
    {
        vTaskDelay(3);
        if(read_usb_serial(receive_buffer))
        {
            cltool_receiveln((char*)receive_buffer);
            if(receive_buffer[0] == '\r') printf("\r\n\r");
            if(receive_buffer[0] == '\b') printf("\b\x20\b\n");
            if(receive_buffer[0] != '\b' && receive_buffer[0] != '\r') printf("%s\n", receive_buffer);
        }
        cltool_run_scheduled();
    }
}



int main(void)
{
    

    configure_usb_serial();
    while(!usb_serial_available());
    
    //Start the scheduler with the main program
    xTaskCreate(main_task, "main_task", 1024,(void*)NULL, 2, NULL);
    vTaskStartScheduler();


    //Should not reach here
    delay_1ms(3000);
    printf("Error: scheduler failed! Reached end of main.\r\n\r");
    while(1);
    
    
}


