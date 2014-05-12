#include "Keyboard.h"
#include <iostream>
#include <windows.h>
#include <QDebug>

#define NUMKEYS 104

int i;
int idle;
bool full;

Keyboard::keyData keyStorage[NUMKEYS] = {            //KEYS                                                          ///INDEX
                                       0xA0,7,99,46,116, 0xC0,8,32,23,48, 0x1B,7,7,22,23, //left shift, tilde, escape                     :: 0,1,2
                                       0x70,48,7,63,23, 0x71,68,7,83,23, 0x72,89,7,104,23, //F1-3                                          :: 3,4,5
                                       0x73,109,7,123,23, 0x74,141,7,156,23, 0x75,161,7,177,23, //F4-6                                          :: 6,7,8
                                       0x76,182,7,197,23, 0x77,202,7,217,23, 0x78,233,7,248,23, //F7-9                                          :: 9,10,11
                                       0x79,253,7,269,23, 0x7A,274,7,289,23, 0x7B,294,7,310,23, //F10-12                                        :: 12,13,14
                                       0x30,212,32,228,48, 0x31,28,32,43,48, 0x32,49,32,64,48,  //0-2                                           :: 15,16,17
                                       0x33,69,32,84,48, 0x34,89,32,105,48, 0x35,110,32,125,48,  //3-5                                           :: 18,19,20
                                       0x36,130,32,146,48, 0x37,150,32,166,48, 0x38,171,32,186,48,  //6-8                                           :: 21,22,23
                                       0x39,192,32,207,48, 0x41,42,77,58,93, 0x42,134,99,149,116,  //9, A, B                                       :: 24,25,26
                                       0x43,93,99,108,116, 0x44,83,77,98,93, 0x45,79,54,95,70,  //C, D, E                                       :: 27,28,29
                                       0x46,103,77,119,93, 0x47,124,77,139,93, 0x48,144,77,161,93,  //F, G, H                                       :: 30,31,32
                                       0x49,182,54,198,70, 0x4A,165,77,181,93, 0x4B,185,77,201,93,  //I, J, K                                       :: 33,34,35
                                       0x4C,206,77,222,93, 0x4D,174,99,190,116, 0x4E,154,99,170,116,  //L, M, N                                       :: 36,37,38
                                       0x4F,203,54,218,70, 0x50,223,54,239,70, 0x51,38,54,54,70,  //O, P, Q                                       :: 39,40,41
                                       0x52,100,54,116,70, 0x53,62,77,78,93, 0x54,120,54,137,70,  //R, S, T                                       :: 42,43,44
                                       0x55,162,54,177,70, 0x56,113,99,129,116, 0x57,59,54,74,70,  //U, V, W                                       :: 45,46,47
                                       0x58,72,99,88,116, 0x59,141,54,157,70, 0x5A,51,99,67,116,  //X, Y, Z                                       :: 48,49,50
                                       0x08,274,31,313,48, 0x09,7,54,33,70, 0x0D,268,77,312,93,  //backspace, tab, enter                         :: 51,52,53
                                       0x14,7,77,37,93, 0x20,86,122,203,139, 0x5B,34,122,56,139,  //capslock, space, left windows                 :: 54,55,56
                                       0x5C,234,122,255,139, 0x5D,259,122,281,139, 0x90,401,31,416,48,  //right windows, option key, numlock            :: 57,58,59
                                       0x60,401,122,437,139, 0x61,401,99,416,116, 0x62,421,99,437,116,  //numpad 0-2                                    :: 60,61,62
                                       0x63,442,99,457,116, 0x64,401,77,416,93, 0x65,421,77,437,93,  //numpad 3-5                                    :: 63,64,65
                                       0x66,442,77,458,93, 0x67,401,54,416,70, 0x68,421,54,437,70,  //numpad 6-8                                    :: 66,67,68
                                       0x69,442,54,457,70, 0x6A,442,31,457,48, 0x6B,462,54,478,92,  //numpad 9, numpad multiply, numpad add         :: 69,70,71
                                       0x6C,462,99,478,139, 0x6D,462,31,478,48, 0x6E,442,122,457,139,  //numpad enter, numpad subract, numpad decimal  :: 72,73,74
                                       0x6F,421,31,437,48, 0x21,369,31,385,48, 0x22,369,54,385,70,  //numpad divide, page up, page down             :: 75,76,77
                                       0x23,349,54,364,70, 0x24,349,31,364,48, 0x25,330,122,345,138,  //end, home, left arrow                         :: 78,79,80
                                       0x26,350,99,366,116, 0x27,371,122,386,138, 0x28,350,122,366,138,  //up arrow, right arrow, down arrow             :: 81,82,83
                                       0x2C,328,7,343,23, 0x2D,328,31,344,48, 0x2E,328,54,344,70,  //printscreen, insert, delete                   :: 84,85,86
                                       0xA1,258,99,312,116, 0xA2,7,122,30,139, 0xA3,285,122,312,139,  //right shift, left control, right control      :: 87,88,89
                                       0xBA,227,77,242,93, 0xBB,253,31,269,48, 0xBC,195,99,210,116,  //semicolon, plus, comma                        :: 90,91,92
                                       0xBD,232,31,248,48, 0xBE,216,99,231,116, 0xBF,236,99,251,116,  //minus, period, backslash /                    :: 93,94,95
                                       0xDB,244,54,259,70, 0xDC,286,54,313,70, 0xDD,264,54,280,70,  //left bracket, forwardslash \, right bracket   :: 96,97,98
                                       0x91,348,7,363,23, 0xA4,61,122,82,139, 0xA5,207,122,229,139,  //scroll lock, left alt, right alt              :: 99,100,101
                                       0xDE,247,77,263,93, 0x13,368,7,384,23};     //quotes, pause break                           :: 102,103

Keyboard::Keyboard() {
    i = 0;

    for (int i = 0; i < 100; i++) {
        keys[i].id = 0;
        keys[i].x = 0;
        keys[i].y = 0;
        keys[i].width = 0;
        keys[i].height = 0;
    }
}


void Keyboard::getKeys(){
        for(int j = 0; j < NUMKEYS; j++)
        {
            if(keys[i-1].id == (keys[i-2]).id)
            {
                i--;
            }
            if(GetAsyncKeyState(keyStorage[j].id) & 0x8000)
            {
                keys[i] = keyStorage[j];
                idle = 0;
                i++;
            }
            else
            {
                idle++;
            }
            if(i > 20)
            {
                i = 0;
                full = true;
            }
        }
        Sleep(100);
        //qDebug() << keys[i].id;
}
