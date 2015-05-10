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
#include "MidiCommands.h"
#include "Statistic.h"

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))
#define SH_MAX_DISTANCE 150

static int compar(const void* a, const void* b)
{
    return (*(int*)a - *(int*)b);
}
#if 0
double SensorHandler::computeStdDev(int start, int end, int avg) const
{
    double ret;
    double sum = 0;
    for (int i = start; i < end; i++)
    {
        sum += (_pinReads[i] - avg) * (_pinReads[i] - avg);
    }

    ret = sqrt(sum / (end - start));
    if (isnan(ret))
    {
        for (int i = start; i < end; i++)
        {
            Serial1.print(_pinReads[i]);
            Serial1.print(",");
        }
        Serial1.print("sizeof: ");
        Serial1.print(sizeof(double));
        Serial1.println(sum);
    }
    return ret;
}
#else
double SensorHandler::computeStdDev(int start, int end, int avg) const
{
}
#endif

int SensorHandler::readAvgPin(int pin, double *stdDev)
{
    int total = -1;
    // Read the analog pin multiple times.
    // Then sort all of those reads.
    // Then throw away the highest quarter, and the lowest quarter
    // Then average the remaining half samples
    // to try to get a stable analog input
    _pinReads[_pinReadsIndex] = analogRead(pin);
    _pinReadsIndex++;
    if (_pinReadsIndex == ARRAY_SIZE(_pinReads))
    {
        Statistic stats;
        qsort(_pinReads, ARRAY_SIZE(_pinReads), sizeof(_pinReads[0]), compar);
        int start = ARRAY_SIZE(_pinReads) / 4;
        int end = ARRAY_SIZE(_pinReads) - start;
        for (int i = start; i < end; i++)
        {
            stats.add(_pinReads[i]);
        }
        _pinReadsIndex = 0;
        *stdDev = stats.pop_stdev();
        total = stats.average();
    }

    return total;
}

int SensorHandler::scaleDistance(int distance, unsigned long max) const
{
    unsigned long d = distance;
    d *= max;
    d /= 1023;
    return d;
}

int SensorHandler::getDistance(double *stdDev)
{
    int ret = readAvgPin(_pin, stdDev);
    if (((ret < SH_MAX_DISTANCE) || (*stdDev > 5.0)) && (ret != -1))
    {
        ret = 0;
    }
    return ret;
}

int SensorHandler::getNote()
{
    return _note;
}

void SensorHandler::switchModulationType()
{
    switch (_mode)
    {
        case SHM_PITCH:
            _mode = SHM_BRIGHTNESS;
            Serial1.println("Brightness");
            break;
        case SHM_BRIGHTNESS:
            _mode = SHM_TIMBRE;
            Serial1.println("Timbre");
            break;
        case SHM_TIMBRE:
            _mode = SHM_PITCH;
            Serial1.println("Pitch");
            break;
    }
}

void SensorHandler::switchNote()
{
    _note = (_note + 1) % 128;
    Serial1.print("SetNote: ");
    Serial1.println(_note);
}

bool SensorHandler::getNoteOnCommand(const int distance, byte* b1, byte* b2, byte* b3)
{
    bool sendCmd = false;
    *b1 = MM_NOTE_ON;
    *b2 = getNote();
    if (distance == 0)
    {
        *b3 = MM_MIN_ATTACK;
        if (_notePlaying == true)
        {
            sendCmd = true;
        }
        _notePlaying = false;

    }
    else
    {
        *b3 = MM_MAX_ATTACK;
        _notePlaying = true;
        sendCmd = true;
    }
    return sendCmd;
}

bool SensorHandler::getPitchCommand(const int distance, byte* b1, byte* b2, byte* b3)
{
    int d = scaleDistance(distance, 0x3fff);
    *b1 = MM_PITCH_WHEEL;
    *b2 = d & 0x7f;
    *b3 = (d >> 7) & 0x7f;
    return true;
}

bool SensorHandler::getBrightnessCommand(const int distance, byte* b1, byte* b2, byte* b3)
{
    *b1 = MM_CONTROL_CHANGE;
    *b2 = MM_CONTROL_BRIGHTNESS;
    *b3 = scaleDistance(distance, 0x7f) + 25;
    return true;
}

bool SensorHandler::getTimbreCommand(const int distance, byte* b1, byte* b2, byte* b3)
{
    *b1 = MM_CONTROL_CHANGE;
    *b2 = MM_CONTROL_TIMBRE;
    *b3 = scaleDistance(distance, 0x7f) + 25;
    return true;
}

bool SensorHandler::getCommand(byte* b1, byte* b2, byte* b3)
{
    double stdDev;
    bool sendCmd = false;
    int distance = getDistance(&stdDev);
    if ((distance != -1) && ((millis() - _t0) > 10))
    {
        _t0 = millis();
        if ((_notePlaying == false) || (distance == 0))
        {
            sendCmd = getNoteOnCommand(distance, b1, b2, b3);
        }
        else
        {
            switch (_mode)
            {
                case SHM_PITCH:
                    sendCmd = getPitchCommand(distance, b1, b2, b3);
                    break;
                case SHM_BRIGHTNESS:
                    sendCmd = getBrightnessCommand(distance, b1, b2, b3);
                    break;
                case SHM_TIMBRE:
                    sendCmd = getTimbreCommand(distance, b1, b2, b3);
                    break;
            }
        }
        if ((sendCmd == true) && (distance != 0))
        {
            debugMidiOut(distance, stdDev, *b1, *b2, *b3);
        }
    }
    return sendCmd;
}

void SensorHandler::printHex(byte h) const
{
    if (h < 0x10)
    {
        Serial1.print("0");
    }
    Serial1.print(h, HEX);
}

void SensorHandler::debugMidiOut(int distance, double stdDev, byte cmd, byte data1, byte data2) const
{
    Serial1.print(_name);
    Serial1.print(": ");
    printHex(cmd);
    Serial1.print(" ");
    if ((cmd & MM_STATUS_MASK) == MM_PITCH_WHEEL)
    {
        unsigned int pitch;
        pitch = data2;
        pitch = (pitch << 7) | data1;
        printHex(pitch);
        Serial1.print("   ");
    }
    else
    {
        printHex(data1);
        if (((cmd & MM_STATUS_MASK) != MM_CHANNEL_PRESSURE) && ((cmd & MM_STATUS_MASK) != MM_PATCH_CHANGE))
        {
            Serial1.print(" ");
            printHex(data2);
        }
        else
        {
            Serial1.print("   ");
        }
    }
    Serial1.print(" Distance: ");
    if (distance < 100)
    {
        Serial1.print("0");
    }
    if (distance < 10)
    {
        Serial1.print("0");
    }
    Serial1.print(distance);
    Serial1.print(" stdDev: ");
    Serial1.print(stdDev);
    Serial1.println("");
}
