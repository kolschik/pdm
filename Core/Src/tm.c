#include "tm.h"
static const uint32_t lut_start = 155;
const uint32_t tempr_scale = 100;
static const uint32_t lut_step = 5;

uint32_t tm_lut[] = {160, 179, 202, 227, 257, 292, 333, 379,
    427, 491, 567, 657, 763, 890, 1044, 1228,
    1452, 1725, 2058, 2466, 2968,  3588, 4357, 5318,
    6523, 8047, 10000, 12461, 15652, 19783, 25152, 32116,
    41306, 53280, 68982, 89682, 117280 };

uint32_t tm_convert(uint32_t Rtm) {   
    int high = 36;
    int low = 0;
    if((Rtm < tm_lut[low]) || (Rtm > tm_lut[high])){
        return UINT32_MAX;
    }
    int mid_idx;
    while (1) {
        mid_idx = (low + high) / 2;
        if (Rtm < tm_lut[mid_idx]) {
            high = mid_idx;
        } else if (Rtm > tm_lut[mid_idx]) {
            low = mid_idx;
        } else {
            break;
        }
        if (high - low <= 1) {
            mid_idx = low;
            break;
        }
    }
    int temper = lut_start * tempr_scale - lut_step * tempr_scale * mid_idx - 
        lut_step * tempr_scale * (Rtm - tm_lut[mid_idx]) / (tm_lut[mid_idx+1] - tm_lut[mid_idx]) ;
    return 0;
}