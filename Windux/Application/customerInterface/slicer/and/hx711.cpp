#include "hx711.h"
#include <stdint.h>
#include <sys/types.h>
#include <sched.h>
#include <string.h>
#include <stdlib.h>
#include <wiringPi.h>

#define CLOCK_PIN       15
#define DATA_PIN        16
#define N_SAMPLES       8 //the number of samples that get averaged per run()
#define SPREAD          10 //the spread of numbers from the average that is allowed
#define SCK_ON  (GPIO_SET0 = (1 << CLOCK_PIN))
#define SCK_OFF (GPIO_CLR0 = (1 << CLOCK_PIN))
#define DT_R    (GPIO_IN0  & (1 << DATA_PIN))

unsigned long  read_cnt(long offset, int argc);
void           set_gain(int r);
void           setHighPri(void);
int data_pin = DATA_PIN;
int clock_pin = CLOCK_PIN;
long offset;
long data_read;
float weight;
float cal_num = 285; //a constant used for calibrating weight
int temp = 0;
#define NUM_TAPS 15 //length of FIP Low Pass Filter

//coefficients for the Low Pass Filter
const long h2[2*NUM_TAPS] = {0xFF8FEAA9, 0x017D8251, 0x02E9C627, 0xF9F9D2C1,
                            0xF127126F, 0x0C7B3D35, 0x4C493555, 0x7049DAF1,
                            0x4C493555, 0x0C7B3D35, 0xF127126F, 0xF9F9D2C1,
                            0x02E9C627, 0x017D8351, 0xFF8FEAA9, 0xFF8FEAA9,
                            0x017D8351, 0x02E9C627, 0xF9F9D2C1, 0xF127126F,
                            0x0C7B3D35, 0x4C493555, 0x7049DAF1, 0x4C493555,
                            0x0C7B3D35, 0xF127126F, 0xF9F9D2C1, 0x02E9C627,
                            0x017D8351, 0xFF8FEAA9};

//delay array for the low pass filter
long z[NUM_TAPS] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

// This is a Low Pass Filter that filters the raw counts that come out
// of the AtoDConverter
void hx711::FIRFilter(CountsStruct *pCounts)
{
    //pCounts = (CountsStruct*)malloc(100);
    char i;
    static signed char state = 0;
    int64_t accum;
    const long *p_h;
    long *p_z;
    long ConvertedADCounts;
    long FilteredOutput;

    //Set Counts to zero to avoid bad data
    pCounts->AtoDCounts = 0;

    pCounts->AtoDCounts = run();

    ConvertedADCounts = pCounts->AtoDCounts;

    z[state] = ConvertedADCounts;

    p_h = h2 + NUM_TAPS - state;
    p_z = z;
    accum = 0;

    for (i = 0; i < NUM_TAPS; i++)
    {
        accum += ((int64_t)(*p_h++) * (int64_t)(*p_z++));
    }

    if (--state < 0)
    {
        state += NUM_TAPS;
    }

    FilteredOutput = (AtoDType)(accum >> 32);

    if (FilteredOutput < 0)
    {
        FilteredOutput = 0;
    }

    pCounts->FIRCounts = (ulong)FilteredOutput;
}

// main constructor for the hx711 AtoDConverter
hx711::hx711()
{
    offset = 0;
    sleepEnable = true; //if the system can sleep
    setSnapMultiplier(0.9); //how fast snap can change
    analogResolution = 16777216; //24-bit number
    activityThreshold = 2000.0; //how far apart the raw values must be from each other to trigger activity while the scale is sleeping
    edgeSnapEnable = true; //if the system will try to get closer to a max value when it goes into sleep mode
    errorEMA = 0.0;
    sleeping = false; //if the system is currently sleeping
}

// sets the Priority of the hx711 to high within memory
void hx711::setHighPri(void)
{
   struct sched_param sched;

   memset(&sched, 0, sizeof(sched));

   sched.sched_priority = 10;
}

// sets up the gpio for wiringPi
void hx711::setup_gpio()
{
   pinMode(clock_pin, OUTPUT);
   pinMode(data_pin, INPUT);
   digitalWrite(clock_pin, LOW);
}

// this gets an average for the number of NSAMPLES specified
float hx711::run()
{
   int readings;
   long tmp_avg = 0;
   long tmp_avg2;
   float filter_low, filter_high;
   float spread_percent = SPREAD/100.0/2.0;
   int nsamples = N_SAMPLES;
   long samples[nsamples];

   setHighPri();
   setup_gpio();

      readings = 0;
      samples[0] = read_cnt(0);

      for(int i = 0;i<nsamples;i++) {
         samples[i] = read_cnt(0);
         tmp_avg += samples[i];
      }

      tmp_avg = tmp_avg/nsamples;

      tmp_avg2 = 0;
      readings = 0;

      filter_low = (float)tmp_avg*(1.0 - spread_percent);
      filter_high = (float)tmp_avg*(1.0 + spread_percent);

      for(int i = 0;i<nsamples;i++) {
         if((samples[i] < filter_high && samples[i] > filter_low) || (samples[i] > filter_high && samples[i] < filter_low)) {
            tmp_avg2 += samples[i];
            readings++;
         }
      }

      if(readings == 0) {
         return weight;
      }

      weight = (tmp_avg2/readings-offset);
      return weight;
}

// this method is not currently being used
u_int16_t hx711::sizecvt(int read)
{
   if(read > 255 || read < 0)
   {
      //exit(EXIT_FAILURE)
   }
   return (u_int16_t)read;
}

// this gets an actual raw count from the AtoDConverter in 24-bit form
unsigned long hx711::read_cnt(long curr_offset) {
   long count = 0;

   while(sizecvt(digitalRead(data_pin)));
   digitalWrite(clock_pin, LOW);
   delayMicroseconds(2000);

   for(int i=0;i<24;i++) {
      digitalWrite(clock_pin, HIGH);
      delayMicroseconds(1);
      count = count << 1;
      if(sizecvt(digitalRead(data_pin)) > 0) {count++;}
      digitalWrite(clock_pin, LOW);
      delayMicroseconds(1);
   }

   digitalWrite(clock_pin, HIGH);
   delayMicroseconds(1);
   digitalWrite(clock_pin, LOW);
   delayMicroseconds(1);

   if (count & 0x800000) {
      count |= (long) ~0xffffff;
   }

   data_read = count;
   return(count - curr_offset);
}

// returns the current offset
long hx711::get_offset()
{
    return data_read;
}

//changes the offset to the number passed to it
void hx711::set_offset(long new_offset)
{
    offset = new_offset;
}

// returns the current weight variable
float hx711::get_weight()
{
    return weight;
}

// tares the scale
void hx711::tare()
{
    offset = data_read;
}

// starts the sleep function for determining when the scale should be sleeping
void hx711::update(int rawValue)
{
    prevResponsiveValue = responsiveValue;
    responsiveValue = getResponsiveValue(rawValue);
    responsiveValueHasChanged = responsiveValue != prevResponsiveValue;
}

// sleep function along with some other number manipulation
int hx711::getResponsiveValue(int newValue)
{
    if(sleepEnable && edgeSnapEnable) {
        if(newValue < activityThreshold) {
            newValue = (newValue*2) - activityThreshold;
        }
        else if(newValue > analogResolution - activityThreshold) {
            newValue = (newValue * 2) - analogResolution + activityThreshold;
        }
    }

    unsigned int diff = abs(newValue - smoothValue);


    errorEMA += ((newValue - smoothValue) - errorEMA)*0.4;


    if(sleepEnable) {
        sleeping = abs(errorEMA) < activityThreshold;
    }


    if(sleepEnable && sleeping) {
        return (int)smoothValue;
    }

    float snap = snapCurve(diff*snapMultiplier);

    if(sleepEnable) {
        snap = snap*0.5 + 0.5;
    }

    smoothValue += (newValue - smoothValue)*snap;

    if(smoothValue < 0.0) {
        smoothValue = 0.0;
    }
    else if(smoothValue > analogResolution - 1) {
        smoothValue = analogResolution - 1;
    }

    return (int)smoothValue;
}

// uses a hyberbolic function to calculate the "snap" for the function
// which is how fast the value responds to an actual change on the scale
float hx711::snapCurve(float x)
{
    float y = 1.0/(x+1.0);
    y = (1.0-y)*2.0;
    if(y>1.0) {
        return 1.0;
    }
    return y;
}

// this is how fast the snap variable can change.  A higher value means
// snap can change faster and a lower value means it changes slower
void hx711::setSnapMultiplier(float newMultiplier)
{
    if(newMultiplier > 1.0) {
        newMultiplier = 1.0;
    }
    if(newMultiplier < 0.0) {
        newMultiplier = 0.0;
    }
    snapMultiplier = newMultiplier;
}

