#ifndef ESP_C
#define ESP_C
#include <stdint.h>
#include <stdio.h> 
#include <iostream>
#include "procfunc.h"
using namespace std;

HBRUSH EnemyBrush = CreateSolidBrush(RGB(255,0,0));


Vector3 screenpos;
Vector3 screenhead;


void DrawFilledRect(int x, int y, int w, int h)
{
	RECT rect = { x, y, x + w, y + h };
	FillRect(hdc, &rect, EnemyBrush);

}
 
void DrawBorderBox(int x, int y, int w, int h, int thickness)
{
	DrawFilledRect(x, y, w, thickness); 
	DrawFilledRect(x, y, thickness, h); 
	DrawFilledRect((x + w), y, thickness, h); 
	DrawFilledRect(x, y + h, w + thickness, thickness); 
}
 



int doEsp() {
    uintptr_t playerclass = (modulebase+PLAYERCLASS);
    uintptr_t entityclass = (modulebase+ENTITYCLASS);
    uintptr_t matrixclass = (modulebase+VIEWMATRIX);
    players* parray;
    int amouth = GetPlayersAmouth()-1;
    Vector3 pos;
    Vector3 head;
    players pinfo = GetPlayerInfo(playerclass);
    float *vm;
    float height = 0;
    float width = 0;
    vm = GetMatrix(matrixclass);
    while (1)
    {
        GetSWindowsSize();
        amouth = GetPlayersAmouth()-1;
        parray = GetPlayerList(entityclass, amouth);
        for (size_t i = 0; i < amouth; i++)
        {
            vm = GetMatrix(matrixclass);
            head.x = parray[i].xyz[0];
            head.y = parray[i].xyz[1];
            head.z = parray[i].xyz[2];
            pos.x = head.x;
            pos.y = head.y;
            pos.z = head.z-4.5;
            WorldToScreen(pos,screenpos, vm,screenX,screenY);
            WorldToScreen(head,screenhead, vm,screenX,screenY);

            height = screenhead.y - screenpos.y;
            width = height / 2.4f;         
            //printf("[%.f %.f %.f] [%.f %.f]\n",pos.x,pos.y,pos.z, screenpos.x, screenpos.y);
            if(parray[i].vie[0] > 0){
                DrawLine(screenX / 2, screenY, screenpos.x, screenpos.y);
                DrawBorderBox(screenpos.x - (width / 2), screenpos.y, width, height, 1);   

                }
        }
    }
    return 1;
}
#endif