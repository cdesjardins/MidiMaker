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

#ifndef MM_MIDI_HANDLER
#define MM_MIDI_HANDLER

class MidiHandler
{
public:
    MidiHandler()
    {
        Serial.begin(31250);
    }
    void sendMidi(byte cmd, byte data1, byte data2);
    bool recvMidi(byte &cmd, byte &data1, byte &data2);
    
    void process();
    
private:
    byte getByte();
};

#endif