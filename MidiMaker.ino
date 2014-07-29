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
    _statusLedB(MM_STAT_LED_2)
    {
        _sensorsHandler.push_back(new SensorHandler(MM_SENSOR_1));
        _sensorsHandler.push_back(new SensorHandler(MM_SENSOR_2));
        /*
        _sensorsHandler.push_back(new SensorHandler(MM_SENSOR_3));
        _sensorsHandler.push_back(new SensorHandler(MM_SENSOR_4));
        _sensorsHandler.push_back(new SensorHandler(MM_SENSOR_5));
        _sensorsHandler.push_back(new SensorHandler(MM_SENSOR_6));
        _sensorsHandler.push_back(new SensorHandler(MM_SENSOR_7));
        _sensorsHandler.push_back(new SensorHandler(MM_SENSOR_8));
        */
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
    
    /* Debug code */
    bool button(int button_num)
    {
        return (!(digitalRead(button_num)));
    }

    void setupModes()
    {
        static unsigned long _t0 = 0;
        static int sensorSelect = 0;
        if ((millis() - _t0) > 100)
        {
            _t0 = millis();
            if (button(MM_BUTTON1))
            {
                _sensorsHandler[sensorSelect]->switchNote();
            }
            if (button(MM_BUTTON2))
            {
                _sensorsHandler[sensorSelect]->switchModulationType();
            }
            if (button(MM_BUTTON3))
            {
                sensorSelect = (sensorSelect + 1) % _sensorsHandler.size();
                Serial1.print("Sensor selected: ");
                Serial1.println(sensorSelect);
            }
        }
    }
    void processSensors()
    {
        std::vector<SensorHandler*>::iterator it;
        for (it = _sensorsHandler.begin(); it != _sensorsHandler.end(); it++)
        {
            byte b1, b2, b3;
            if ((*it)->getCommand(&b1, &b2, &b3) == true)
            {
                _midiHandler.sendMidi(b1, b2, b3);
            }
        }
    }
    
    void process()
    {
        _statusLedA.process();
        _statusLedB.process();
        _midiHandler.process();
        processSensors();
        setupModes();
    }
    
private:
    StatusLed _statusLedA;
    StatusLed _statusLedB;
    MidiHandler _midiHandler;
    std::vector<SensorHandler*> _sensorsHandler;
};

MidiMaker *g_midiMaker;

void setup()
{
    pinMode(MM_BUTTON1, INPUT);
    pinMode(MM_BUTTON2, INPUT);
    pinMode(MM_BUTTON3, INPUT);

    digitalWrite(MM_BUTTON1, HIGH);
    digitalWrite(MM_BUTTON2, HIGH);
    digitalWrite(MM_BUTTON3, HIGH);

    Serial1.begin(115200);
    g_midiMaker = new MidiMaker();
}

void loop()
{
    g_midiMaker->process();
}