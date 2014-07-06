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

#ifndef MM_STATUS_LED
#define MM_STATUS_LED

class StatusLed
{
public:
    StatusLed(int pin)
        : _pin(pin),
        _numBlinks(0)
    {
        pinMode(_pin, OUTPUT);
    }
    
    void blink(int numBlinks = 10)
    {
        _numBlinks = numBlinks;
        _t0 = millis();
    }
    
    void process()
    {
        if (_numBlinks > 0)
        {
            if ((millis() - _t0) > 30)
            {
                _numBlinks--;
                digitalWrite(_pin, (_numBlinks & 1) ? LOW : HIGH);
                _t0 = millis();
            }
        }
    }
private:
    int _pin;
    unsigned long _t0;
    int _numBlinks;
};

#endif