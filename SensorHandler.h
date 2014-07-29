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

#ifndef MM_SENSOR_HANDLER
#define MM_SENSOR_HANDLER

enum SensorHandlerMode
{
    SHM_PITCH,
    SHM_BRIGHTNESS,
    SHM_TIMBRE,
};

class SensorHandler
{
public:
    SensorHandler(int pin)
    :_pin(pin),
    _t0(0),
    _notePlaying(false),
    _note(0x3C),
    _mode(SHM_PITCH)
    {
        analogReference(EXTERNAL);
    }
    bool getCommand(byte* b1, byte* b2, byte* b3);
    void switchNote();
    void switchModulationType();
private:
    int getDistance() const;
    int getNote();
    bool getNoteOnCommand(const int distance, byte* b1, byte* b2, byte* b3);
    bool getPitchCommand(const int distance, byte* b1, byte* b2, byte* b3);
    bool getBrightnessCommand(const int distance, byte* b1, byte* b2, byte* b3);
    bool getTimbreCommand(const int distance, byte* b1, byte* b2, byte* b3);

    int scaleDistance(int distance, unsigned long max) const;
    int readAvgPin(int pin) const;
    int _pin;
    bool _notePlaying;
    unsigned long _t0;
    int _note;
    SensorHandlerMode _mode;
};
#endif