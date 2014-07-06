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
#include "MidiHandler.h"

void MidiHandler::sendMidi(byte cmd, byte data1, byte data2)
{
    Serial.write(cmd);
    Serial.write(data1);
    Serial.write(data2);
    
    Serial1.print("Write: ");
    Serial1.print(cmd, HEX);
    Serial1.print(" ");
    Serial1.print(data1, HEX);
    Serial1.print(" ");
    Serial1.println(data2, HEX);
}

bool MidiHandler::recvMidi(byte &cmd, byte &data1, byte &data2)
{
    bool ret = false;
    if (Serial.available() > 0)
    {
        cmd = Serial.read();
        switch (cmd)
        {
            default:
            case 0xf0:
            case 0xf2:
                data1 = getByte();
                data2 = getByte();
                break;
            case 0xc0: case 0xc1: case 0xc2: case 0xc3: case 0xc4: case 0xc5: case 0xc6: case 0xc7: case 0xc8: case 0xc9:
            case 0xca: case 0xcb: case 0xcc: case 0xcd: case 0xce: case 0xcf: case 0xd0: case 0xd1: case 0xd2: case 0xd3:
            case 0xd4: case 0xd5: case 0xd6: case 0xd7: case 0xd8: case 0xd9: case 0xda: case 0xdb: case 0xdc: case 0xdd:
            case 0xde: case 0xdf: case 0xf1: case 0xf3:
                data1 = getByte();
                break;
            case 0xf4:
                Serial.println("Error: Rx 0xf4");
                break;
            case 0xf5:
                Serial.println("Error: Rx 0xf5");
                break;
            case 0xf6: case 0xf7: case 0xf8: case 0xf9: case 0xfa: case 0xfb: case 0xfc: case 0xfd: case 0xfe: case 0xff:
                break;
        }
        ret = true;
    }
    return ret;
}

byte MidiHandler::getByte()
{
    while (Serial.available() == 0)
    {
    }
    return Serial.read();
}

void MidiHandler::process()
{
    byte cmd;
    byte data1;
    byte data2;
    if ((recvMidi(cmd, data1, data2) == true) && (cmd < 0xf0))
    {
        Serial1.print("MidiCommand: 0x");
        Serial1.print(cmd, HEX);
        Serial1.print(" 0x");
        Serial1.print(data1, HEX);
        Serial1.print(" 0x");
        Serial1.println(data2, HEX);

    }
}