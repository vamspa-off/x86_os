#include "string.h"
#include "controller.h"
#include "string.h"

// Function to read from RTC
unsigned char rtc_read(unsigned char reg) {
    while (inb(0x71) & 0x80); // Check bit 7 (UIP) of register A

    // Read the value from the specified register
    outb(0x70, reg);
    return inb(0x71);
}

int get_unix_time(){
    unsigned long unix_time = 0;

    int hours = (int) rtc_read(0x04);
    int minutes = (int) rtc_read(0x02);
    int seconds = (int) rtc_read(0x00);

    int seconds_h = (seconds >> 4) & 0x0F;
    int seconds_l = seconds & 0x0F;

    int new = (seconds_h)*10 + seconds_l;

    // Calculate Unix time by adding seconds for hours, minutes, and seconds
    unix_time += hours * 3600; // Hours to seconds
    unix_time += minutes * 60; // Minutes to seconds
    unix_time += new; // Seconds

    return unix_time;
}

void bcd_to_string(unsigned char reg,unsigned char* high,unsigned char* low){
    unsigned char bcd = rtc_read(reg);
    intToString((unsigned int)(bcd >> 4) & 0x0F, high);
    intToString((unsigned int)bcd & 0x0F, low);
}

void get_time(unsigned char* buffer){
    int buf_size = 0;

    unsigned char hour_high[3],hour_low[3];
    unsigned char min_high[3],min_low[3];
    unsigned char second_high[3],second_low[3];

    bcd_to_string(0x04,hour_high,hour_low);
    bcd_to_string(0x02,min_high,min_low);
    bcd_to_string(0x00,second_high,second_low);

    add_to_string(hour_high,buffer,&buf_size);
    add_to_string(hour_low,buffer,&buf_size);

    add_to_string(min_high,buffer,&buf_size);
    add_to_string(min_low,buffer,&buf_size);

    add_to_string(second_high,buffer,&buf_size);
    add_to_string(second_low,buffer,&buf_size);

}

int get_timestamp() {
    int low, high;
    asm volatile("rdtsc" : "=a" (low), "=d" (high));
    return low;
}

int delay_passed(int* current){
    // true if 2 seconds has passed since the current time pointer
    if (get_unix_time() - *current < 2)return 0;
    return 1;
}

int time(){
    char hour_high[3],hour_low[3];
    unsigned char min_high[3],min_low[3];
    unsigned char second_high[3],second_low[3];

    bcd_to_string(0x04,hour_high,hour_low);
    bcd_to_string(0x02,min_high,min_low);
    bcd_to_string(0x00,second_high,second_low);

    int hour_hi = stringToInt(hour_high) * 10;
    int hours = stringToInt(hour_low) + hour_hi;
    int min_hi = stringToInt(min_high) * 10;
    int min = stringToInt(min_low) + min_hi;
    int sec_hi = stringToInt(second_high) * 10;
    int sec = stringToInt(second_low) + sec_hi;
    return hours * 3600 + min * 60 + sec;
}

void sleep(unsigned int time_to_sleep){
    unsigned int rT = time() + time_to_sleep;
    while(time() < rT);
}
