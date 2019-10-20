#include "utils.h"

void getBatteryVoltage(float *v, float *fraction) {
    *v = (float)analogRead(A0) / 1023.0 * 4.57; // 10k/33k
    *fraction = (*v - 3.5) / 0.7;
    *fraction = *fraction > 1.0 ? 1.0 : *fraction;
}
