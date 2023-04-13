#ifndef PROCFUNC_H
#define PROCFUNC_H
#include <windows.h>
#include <tlhelp32.h> 
#include <shlwapi.h> 
#include <conio.h> 
#include <math.h>


#include "OFFSETS.h"


struct Vector3{
    float x, y, z;
};
struct Vector4{
    float x,y,z,w;
};

struct players {
    char *name[16];
    int vie[1];
    float xyz[3];

};

int GetProcId(const char *name);
bool CALLBACK EnumWindowsProcMy(HWND hwnd,LPARAM lParam);
uintptr_t GetModuleBaseAdress(int procID, const char *modname);
int setupWindowHandles(LPCWSTR processName);
intptr_t GetBaseAddr(intptr_t addr);
intptr_t GetAddr(intptr_t addr);
float *GetMatrix(intptr_t addr);
players* GetPlayerList(intptr_t adress, int amouth);

HWND window = FindWindowA(NULL, "AssaultCube");
HANDLE windowHandle;
DWORD pid = GetProcId("ac_client.exe");

uintptr_t modulebase = GetModuleBaseAdress(pid, "ac_client.exe");
HDC hdc = GetDC(FindWindowA(NULL, "AssaultCube"));
#define EnemyPen RGB(255,0,0)
int screenX = 0;
int screenY = 0;

int GetProcId(const char *name){
    int procId = 0;
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if(hSnap !=INVALID_HANDLE_VALUE){
        PROCESSENTRY32 procentry;
        procentry.dwSize = sizeof(procentry);
        if(Process32First(hSnap, &procentry)){
            do
            {
                if(!strcmp((char *)procentry.szExeFile, name))
                {
                    procId = procentry.th32ProcessID;
                    return procId;
                }
            } while (Process32Next(hSnap, &procentry));
            
        }
    }
    return -1;
}

bool CALLBACK EnumWindowsProcMy(HWND hwnd,LPARAM lParam)
{
    DWORD lpdwProcessId;
    GetWindowThreadProcessId(hwnd,&lpdwProcessId);
    if(lpdwProcessId==lParam)
    {
        window=hwnd;
        return FALSE;
    }
    return TRUE;
}

uintptr_t GetModuleBaseAdress(int procID, const char *modname){
    uintptr_t modBaseAddr = 0;
    HANDLE hSnap =  CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, procID);
    if(hSnap != INVALID_HANDLE_VALUE){
        MODULEENTRY32 modentry;
        modentry.dwSize = sizeof(modentry);
        if(Module32First(hSnap, &modentry)){
            do
            {
                if(!strcmp((char *)modentry.szModule, modname)){
                    modBaseAddr = (uintptr_t)modentry.modBaseAddr;
                    return modBaseAddr;
                    
                }
            } while (Module32Next(hSnap, &modentry));
            
        }
    }
    return 1;
}
int setupWindowHandles(LPCWSTR processName)
{
    window = FindWindowW(0, processName);
    GetWindowThreadProcessId(window, &pid);
    windowHandle = OpenProcess(PROCESS_ALL_ACCESS, false, pid);
    return 1;
}
intptr_t GetBaseAddr(intptr_t addr){

    intptr_t BASEADDR;
    const int buffer[1] = {0};
    ReadProcessMemory(windowHandle, (LPVOID)(addr), (LPVOID)buffer, 4, NULL);
    BASEADDR = buffer[0];
    return BASEADDR;
}
intptr_t GetAddr(intptr_t addr){

    intptr_t value;
    const int buffer[1] = {0};
    ReadProcessMemory(windowHandle, (LPVOID)(addr), (LPVOID)buffer, 4, NULL);
    value = buffer[0];
    return value;
}
float *GetMatrix(intptr_t addr){
    float *matrix = new float[16];
    addr = (addr-(0x6C))+64;
    ReadProcessMemory(windowHandle, (float *)(addr), matrix, 64, NULL);
    return matrix;
}
void GetSWindowsSize(){
    RECT rect;
    if(GetWindowRect(window, &rect))
    {
    screenX = rect.right - rect.left;
    screenY = rect.bottom - rect.top;
    }
}
bool WorldToScreen(Vector3 pos, Vector3 &screen, float *matrix, int windowWidth, int windowHeight)//
{
    //Matrix-vector Product, multiplying world(eye) coordinates by projection matrix = clipCoords
    Vector4 clipCoords;
    clipCoords.x = pos.x*matrix[0] + pos.y*matrix[4] + pos.z*matrix[8] + matrix[12];
    clipCoords.y = pos.x*matrix[1] + pos.y*matrix[5] + pos.z*matrix[9] + matrix[13];
    clipCoords.z = pos.x*matrix[2] + pos.y*matrix[6] + pos.z*matrix[10] + matrix[14];
    clipCoords.w = pos.x*matrix[3] + pos.y*matrix[7] + pos.z*matrix[11] + matrix[15];

    if (clipCoords.w < 0.1f)
        return false;

    //perspective division, dividing by clip.W = Normalized Device Coordinates
    Vector3 NDC;
    NDC.x = clipCoords.x / clipCoords.w;
    NDC.y = clipCoords.y / clipCoords.w;
    NDC.z = clipCoords.z / clipCoords.w;

        //Transform to window coordinates
    screen.x = (windowWidth / 2 * NDC.x) + (NDC.x + windowWidth / 2);
    screen.y = -(windowHeight / 2 * NDC.y) + (NDC.y + windowHeight / 2);
    return true;
}

players GetPlayerInfo(intptr_t addr){   
    intptr_t adress = GetAddr(addr);
    players playerinfo;
    ReadProcessMemory(windowHandle, (LPVOID)(adress+NOM), (LPVOID)playerinfo.name, 16, NULL);
    ReadProcessMemory(windowHandle, (LPVOID)(adress+VIE), (LPVOID)playerinfo.vie, 4, NULL);
    ReadProcessMemory(windowHandle, (float*)(adress+X), &playerinfo.xyz[0], 4, NULL);
    ReadProcessMemory(windowHandle, (float*)(adress+Y), &playerinfo.xyz[1], 4, NULL);
    ReadProcessMemory(windowHandle, (float*)(adress+Z), &playerinfo.xyz[2], 4, NULL);    
    return playerinfo;
}
players* GetPlayerList(intptr_t adress, int amouth){
    players* pArray = new players[amouth];

    intptr_t addr = GetBaseAddr(adress);
    int k = 4;
    intptr_t address = (GetAddr(addr+k));
    for (size_t i = 0; i < amouth; i++)
    {
        ReadProcessMemory(windowHandle, (LPVOID)(address+NOM), (LPVOID)pArray[i].name, 16, NULL);
        ReadProcessMemory(windowHandle, (LPVOID)(address+VIE), (LPVOID)pArray[i].vie, 4, NULL);
        ReadProcessMemory(windowHandle, (float*)(address+X), &pArray[i].xyz[0], 4, NULL);
        ReadProcessMemory(windowHandle, (float*)(address+Y), &pArray[i].xyz[1], 4, NULL);
        ReadProcessMemory(windowHandle, (float*)(address+Z), &pArray[i].xyz[2], 4, NULL);          
        k = k+4;
        address = (GetAddr(addr+k));
    }
    return pArray;
    delete[] pArray;
    
}
int GetPlayersAmouth(){
    intptr_t adress = (modulebase+ENTITYCLASS+PLAYERAMOUTH_OFFSET);
    uint8_t amouth[1];
    ReadProcessMemory(windowHandle, (LPVOID)(adress), (LPVOID)amouth, 1, NULL);
    return amouth[0];

}
void DrawLine(float StartX, float StartY, float EndX, float EndY)
{
	int a, b = 0;
	HPEN hOPen;
	HPEN hNPen = CreatePen(PS_SOLID, 2, EnemyPen);// penstyle, width, color
	hOPen = (HPEN)SelectObject(hdc, hNPen);
	MoveToEx(hdc, StartX, StartY, NULL); //start
	a = LineTo(hdc, EndX, EndY); //end
	DeleteObject(SelectObject(hdc, hOPen));
}

players GetClosestEntity(int amouth, intptr_t entityclass, intptr_t playerclass)
{
    double SmallestDistance = 999999.f;
    players ClosestPlayer;
    players* parray;
    players me = GetPlayerInfo(playerclass);
    parray = GetPlayerList(entityclass, amouth);
    for(int i = 0; i < amouth; i++)
    {  
        double cDistance = sqrt(pow((me.xyz[0]-parray[i].xyz[0]),2) + pow((me.xyz[1]-parray[i].xyz[1]), 2) + pow((me.xyz[2]-parray[i].xyz[2]),2));
        //printf("| %s %f [%f %f %f][%f %f %f]\n", parray[i].name, cDistance,me.xyz[0],me.xyz[1],me.xyz[2],parray[i].xyz[0],parray[i].xyz[1],parray[i].xyz[2]);

        if(cDistance < SmallestDistance)
        {
            SmallestDistance = cDistance;
            ClosestPlayer = parray[i];
        }
    }
    return ClosestPlayer;
   
}





#endif