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
/* This project uses https://github.com/maniacbug/StandardCplusplus as a library */
#include <StandardCplusplus.h>
#include <vector>
#include "MidiHandler.h"
#include "SensorHandler.h"
#include "StatusLed.h"

/* All pins for this project are defined here */

#define MM_BUTTON1      2
#define MM_BUTTON2      3
#define MM_BUTTON3      4
#define MM_STAT_LED_1   7
#define MM_STAT_LED_2   6
#define MM_SENSOR_1     15 /* Analog */
#define MM_SENSOR_2     14 /* Analog */
#define MM_SENSOR_3     13 /* Analog */
#define MM_SENSOR_4     12 /* Analog */
#define MM_SENSOR_5     11 /* Analog */
#define MM_SENSOR_6     10 /* Analog */
#define MM_SENSOR_7     9  /* Analog */
#define MM_SENSOR_8     8  /* Analog */


class MidiMaker
{
public:
    MidiMaker()
    :_statusLedA(MM_STAT_LED_1),
    _statusLedB(MM_STAT_LED_2),
    _t0(0)
    {
        _sensorsHandler.push_back(new SensorHandler(MM_SENSOR_1));
        _sensorsHandler.push_back(new SensorHandler(MM_SENSOR_2));
        _sensorsHandler.push_back(new SensorHandler(MM_SENSOR_3));
        _sensorsHandler.push_back(new SensorHandler(MM_SENSOR_4));
        _sensorsHandler.push_back(new SensorHandler(MM_SENSOR_5));
        _sensorsHandler.push_back(new SensorHandler(MM_SENSOR_6));
        _sensorsHandler.push_back(new SensorHandler(MM_SENSOR_7));
        _sensorsHandler.push_back(new SensorHandler(MM_SENSOR_8));
        _statusLedA.blink(11);
        _statusLedB.blink(10);
    }
    
    ~MidiMaker()
    {
        std::vector<SensorHandler*>::iterator it;
        for (it = _sensorsHandler.begin(); it != _sensorsHandler.end(); it++)
        {
            SensorHandler* sensor = *it;
            delete sensor;
        }
        _sensorsHandler.clear();
    }
    
    void playSounds()
    {
        if ((millis() - _t0) > 100)
        {
            Serial1.print(_sensorsHandler[0]->getDistance());
            Serial1.print("   \r");
            _t0 = millis();
        }
    }
    
    void process()
    {
        _statusLedA.process();
        _statusLedB.process();
        _midiHandler.process();
        std::vector<SensorHandler*>::iterator it;
        for (it = _sensorsHandler.begin(); it != _sensorsHandler.end(); it++)
        {
            (*it)->process();
        }
        playSounds();
    }
    
private:
    StatusLed _statusLedA;
    StatusLed _statusLedB;
    MidiHandler _midiHandler;
    std::vector<SensorHandler*> _sensorsHandler;
    unsigned long _t0;
};

MidiMaker *g_midiMaker;

void setup()
{
    Serial1.begin(115200);
    g_midiMaker = new MidiMaker();
}

void loop()
{
    g_midiMaker->process();
}