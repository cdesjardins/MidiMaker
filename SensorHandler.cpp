/* 
    Copyright (C) 2014 Chris Desjardins - cjd@chrisd.info
    
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <Arduino.h>
#include "SensorHandler.h"

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))

static int compar(const void* a, const void* b)
{
    return (*(int*)a - *(int*)b);
}

int SensorHandler::readAvgPin(int pin) const
{

    // Read the analog pin 40 times.
    // Then sort all of those reads.
    // Then throw away the highest 10, and the lowest 10
    // Then average the remaining 20
    // to try to get a stable analog input
    int pins[20];
    for (int i = 0; i < ARRAY_SIZE(pins); i++)
    {
        pins[i] = analogRead(pin);
    }
    qsort(pins, ARRAY_SIZE(pins), sizeof(pins[0]), compar);
    int start = ARRAY_SIZE(pins) / 4;
    int end = ARRAY_SIZE(pins) - start;
    int total = 0;
    for (int i = start; i < end; i++)
    {
        total += pins[i];
    }
    total /= (end - start);
    return total;
}