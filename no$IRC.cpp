/*
*
*  gbIRC - Control the No$GBA emulator via IRC/TwitchTV.
*  A variant of gbIRC.
*
*  Copyright (C) 2014 AV
*
*  This program is free software: you can redistribute it and/or modify
*  it under the terms of the GNU General Public License as published by
*  the Free Software Foundation, either version 3 of the License, or
*  (at your option) any later version.
*
*  This program is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU General Public License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
*/
#define _WIN32_WINNT 0x0500
#include <winsock2.h>
#include <iostream>
#include <string>
#include <windows.h>
#define MAPVK_VK_TO_VSC 0

#define SERVERIP "199.9.253.199"
#define PORT 6667


using namespace std;

WSAData WSInit;
SOCKET ircSocket;
struct sockaddr_in sockstruct;
char recvBuf [512];
bool ircError;
bool InChannel = false;
string PRIVMSG;
string USER;
string NICK;
string PART;
string PASS = "PASS oauth:ek3a7v2fi3is61g9ij85pa6hwsffk3h \n";
string nick = "n0n0n0n0n0";
string chan = "#n0n0n0n0n0";
string JOIN;

bool ircConnect();
bool ircQuit();
bool ircSend (const char *msg);
void ircMsgHandler();
HWND GBCEmulator = FindWindow("No$dlgClass",0);


bool ircConnect ()
{
    WSADATA wsa_data;
    WSAStartup(MAKEWORD(2, 0), &wsa_data);

    USER = "USER " + nick + " 8 * :" + nick + "\n";
    NICK = "NICK " + nick + "\n";
    JOIN = "JOIN " + chan + "\n";
    WSAStartup (MAKEWORD (2,2), &WSInit);

    ircSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    sockstruct.sin_port = htons(PORT);
    sockstruct.sin_family = AF_INET;
    sockstruct.sin_addr.s_addr = inet_addr(SERVERIP);
    memset(sockstruct.sin_zero,0,8);

    connect (ircSocket ,(sockaddr *) &sockstruct, sizeof(sockaddr));

    Sleep(1600);
    ircSend(PASS.c_str());
    ircSend(NICK.c_str());
    ircSend(USER.c_str());
    ircSend(JOIN.c_str());

    while (!ircError) {
        recv(ircSocket, recvBuf, 512, 0);
        ircMsgHandler();
    }

    return ircQuit();
}


int main()
{
    ircConnect();
    return 0;
}


void ircMsgHandler()
{
    string ircBuffer;
    string pongBuffer;

    for (int i = 0; i<512; i++)
        if(recvBuf[i] == '\n') recvBuf[i++] = '\0';

    ircBuffer = recvBuf;

    cout << ircBuffer.c_str() << endl;


    if(ircBuffer.find("PING :") != string::npos) {
        pongBuffer = "PONG :" + ircBuffer.substr((ircBuffer.find_last_of(":")+1), (ircBuffer.find_last_of("\r")-1)) + "\n";
        ircSend(pongBuffer.c_str());
        if(!InChannel) {
            Sleep(2000);
            ircSend(JOIN.c_str());
            InChannel = true;
        }
    }

    //admin commands

    if(ircBuffer.find(nick+".testserver.local "+"PRIVMSG "+chan+" :!QUIT") != string::npos) {
        ircError = true;
    }


    if(ircBuffer.find(nick+".testserver.local "+"PRIVMSG "+chan+" :!RESTART") != string::npos) {
        ircQuit();
        ircConnect();
    }

    //user commands

    if(ircBuffer.find(":!a") != string::npos) {
        cout << GBCEmulator << endl;
        SetActiveWindow(GBCEmulator);
        SetForegroundWindow(GBCEmulator);
        SetFocus(GBCEmulator);
        cout << GBCEmulator << endl;
        cout << GBCEmulator << endl;

        INPUT ip;
        Sleep(3000);
        ip.type = INPUT_KEYBOARD;
        ip.ki.time = 0;
        ip.ki.wVk = 0;
        ip.ki.dwExtraInfo = 0;


        ip.ki.dwFlags = KEYEVENTF_SCANCODE;
        ip.ki.wScan = 0x1E;  // scan code for A



        SendInput(1, &ip, sizeof(INPUT));
        PRIVMSG = "PRIVMSG " + chan + " :A\n";
        ircSend(PRIVMSG.c_str());


    }

    if(ircBuffer.find(":!b") != string::npos) {
        SetForegroundWindow(GBCEmulator);
        INPUT ip;
        Sleep(3000);
        ip.type = INPUT_KEYBOARD;
        ip.ki.time = 0;
        ip.ki.wVk = 0;
        ip.ki.dwExtraInfo = 0;


        ip.ki.dwFlags = KEYEVENTF_SCANCODE;
        ip.ki.wScan = 0x1F;  //S



        SendInput(1, &ip, sizeof(INPUT));
        PRIVMSG = "PRIVMSG " + chan + " :B\n";
        ircSend(PRIVMSG.c_str());
    }


    if(ircBuffer.find(":!start") != string::npos) {
        SetForegroundWindow(GBCEmulator);
        INPUT ip;
        Sleep(3000);
        ip.type = INPUT_KEYBOARD;
        ip.ki.time = 0;
        ip.ki.wVk = 0;
        ip.ki.dwExtraInfo = 0;


        ip.ki.dwFlags = KEYEVENTF_SCANCODE;
        ip.ki.wScan = 0x1C;  //ENTER


        SendInput(1, &ip, sizeof(INPUT));
        PRIVMSG = "PRIVMSG " + chan + " :Start\n";
        ircSend(PRIVMSG.c_str());
    }

    if(ircBuffer.find(":!up") != string::npos) {
        SetForegroundWindow(GBCEmulator);
        INPUT inp[2];
        memset(inp,0,sizeof(INPUT));

        inp[0].type = INPUT_KEYBOARD;
        inp[0].ki.dwFlags =
            KEYEVENTF_SCANCODE |
            KEYEVENTF_EXTENDEDKEY;
        inp[1] = inp[0];
        inp[1].ki.dwFlags |= KEYEVENTF_KEYUP;
        inp[0].ki.wScan = inp[1].ki.wScan = MapVirtualKey(VK_UP, MAPVK_VK_TO_VSC);
        SendInput(1, inp, sizeof(INPUT));
        Sleep(500);
        SendInput(1, inp+1, sizeof(INPUT));


        PRIVMSG = "PRIVMSG " + chan + " :Up\n";
        ircSend(PRIVMSG.c_str());
    }

    if(ircBuffer.find(":!down") != string::npos) {
        SetForegroundWindow(GBCEmulator);
        INPUT inp[2];
        memset(inp,0,sizeof(INPUT));

        inp[0].type = INPUT_KEYBOARD;
        inp[0].ki.dwFlags =
            KEYEVENTF_SCANCODE |
            KEYEVENTF_EXTENDEDKEY;
        inp[1] = inp[0];
        inp[1].ki.dwFlags |= KEYEVENTF_KEYUP;
        inp[0].ki.wScan = inp[1].ki.wScan = MapVirtualKey(VK_DOWN, MAPVK_VK_TO_VSC);
        SendInput(1, inp, sizeof(INPUT));
        Sleep(500);
        SendInput(1, inp+1, sizeof(INPUT));
        PRIVMSG = "PRIVMSG " + chan + " :Down\n";
        ircSend(PRIVMSG.c_str());
    }

    if(ircBuffer.find(":!left") != string::npos) {
        SetForegroundWindow(GBCEmulator);
        INPUT inp[2];
        memset(inp,0,sizeof(INPUT));

        inp[0].type = INPUT_KEYBOARD;
        inp[0].ki.dwFlags =
            KEYEVENTF_SCANCODE |
            KEYEVENTF_EXTENDEDKEY;
        inp[1] = inp[0];
        inp[1].ki.dwFlags |= KEYEVENTF_KEYUP;
        inp[0].ki.wScan = inp[1].ki.wScan = MapVirtualKey(VK_LEFT, MAPVK_VK_TO_VSC);
        SendInput(1, inp, sizeof(INPUT));
        Sleep(500);
        SendInput(1, inp+1, sizeof(INPUT));
        PRIVMSG = "PRIVMSG " + chan + " :Left\n";
        ircSend(PRIVMSG.c_str());
    }

    if(ircBuffer.find(":!right") != string::npos) {
        SetForegroundWindow(GBCEmulator);
        INPUT inp[2];
        memset(inp,0,sizeof(INPUT));

        inp[0].type = INPUT_KEYBOARD;
        inp[0].ki.dwFlags =
            KEYEVENTF_SCANCODE |
            KEYEVENTF_EXTENDEDKEY;
        inp[1] = inp[0];
        inp[1].ki.dwFlags |= KEYEVENTF_KEYUP;
        inp[0].ki.wScan = inp[1].ki.wScan = MapVirtualKey(VK_RIGHT, MAPVK_VK_TO_VSC);
        SendInput(1, inp, sizeof(INPUT));
        Sleep(500);
        SendInput(1, inp+1, sizeof(INPUT));
        PRIVMSG = "PRIVMSG " + chan + " :Right\n";
        ircSend(PRIVMSG.c_str());
    }

    if(ircBuffer.find(":!select") != string::npos) {
        SetForegroundWindow(GBCEmulator);
        INPUT ip;
        Sleep(3000);
        ip.type = INPUT_KEYBOARD;
        ip.ki.time = 0;
        ip.ki.wVk = 0;
        ip.ki.dwExtraInfo = 0;


        ip.ki.dwFlags = KEYEVENTF_SCANCODE;
        ip.ki.wScan = 0x18;  //O


        SendInput(1, &ip, sizeof(INPUT));
        Sleep(250);
        ip.ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;

        SendInput(1, &ip, sizeof(INPUT));
        PRIVMSG = "PRIVMSG " + chan + " :Select\n";
        ircSend(PRIVMSG.c_str());
    }



}


bool ircQuit ()
{
    shutdown(ircSocket, 2);
    closesocket(ircSocket);
    WSACleanup();

    return true;
}


bool ircSend(const char *msg)
{
    if(send(ircSocket, msg, strlen(msg), 0) == -1) {
        return false;
    } else {
        return true;
    }
}


