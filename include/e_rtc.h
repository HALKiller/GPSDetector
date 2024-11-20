#ifndef E_RTC_H
#define E_RTC_H


#include "Global.h"


#define USE_FULL_SECONDS_FOR_RTC 1

#define HORAS_PER_DAY (uint8_t)24u
#define MINUTES_PER_HOUR (uint8_t)60u
#define SECONDS_PER_MINUTE (uint8_t)60u
#define SECONDS_PER_HOUR (uint16_t)3600u
#if USE_FULL_SECONDS_FOR_RTC
#define SECONDS_PER_DAY (uint32_t)86400u // because of decimo seconds we have a digit more
#else
#define SECONDS_PER_DAY (uint32_t)864000u // because of decimo seconds we have a digit more
#endif

void eRTC_clock_incrementer(void);

void eRTC_clock_reset(void);

void ertc_convert_to_real_time(uint32_t in_time);

void ertc_convert_to_str(void);

uint32_t eRTC_get_second_cnt(void);

void eRTC_clock_sync_to_gps(uint32_t gps_time);

void eRTC_calculate_time_until_tx(void);

// extern volatile uint32_t eRTC_second_cnt;

extern volatile uint8_t tmr4_of_cnt;















































#endif //TEMPLATE