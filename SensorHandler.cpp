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

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))

static int compar(const void* a, const void* b)
{
    return (*(int*)a - *(int*)b);
}

int SensorHandler::readAvgPin(int pin) const
{

    // Read the analog pin multiple times.
    // Then sort all of those reads.
    // Then throw away the highest quarter, and the lowest quarter
    // Then average the remaining half samples
    // to try to get a stable analog input
    int pins[60];
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

int SensorHandler::scaleDistance(int distance, unsigned long max) const
{
    unsigned long d = distance;
    d *= max;
    d /= 1023;
    return d;
}

int SensorHandler::getDistance() const
{
    int ret = readAvgPin(_pin);
    if (ret < 100)
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
    bool sendCmd = false;
    int distance = getDistance();
    if ((millis() - _t0) > 10)
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
    }
    return sendCmd;
}
