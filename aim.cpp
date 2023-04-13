#ifndef AIM_H
#define AIM_H
#include <stdint.h>
#include <stdio.h> 
#include <iostream>
#include "procfunc.h"

/*
CETTE PARTIE N'EST PAS ACHEVEE,
LES FONCTIONS ICI SONT FONCTIONELLES
cette partie reste en debug et n'a jamais été continuée

*/

Vector3 GetPlayerview(intptr_t playerclass){
    intptr_t addr = GetAddr(playerclass);
    Vector3 playerview;
    ReadProcessMemory(windowHandle, (float*)(addr+XPOSCUR), &playerview.x, 4, NULL);
    ReadProcessMemory(windowHandle, (float*)(addr+YPOSCUR), &playerview.y, 4, NULL);  
    printf("%f %f", playerview.x,playerview.y);
    return playerview;
}
int doAim(){
    
    uintptr_t playerclass = (modulebase+PLAYERCLASS);
    uintptr_t entityclass = (modulebase+ENTITYCLASS);
    uintptr_t matrixclass = (modulebase+VIEWMATRIX);
    players entity;
    GetSWindowsSize();
    int amouth = GetPlayersAmouth()-1;
    if(amouth <=0 || amouth > 32){
        printf("Aucun joueur");
        return 1;
    }
    entity = GetClosestEntity(amouth,entityclass, playerclass);
    printf("%s\n", entity.name);
    Vector3 aimscreenpos;
    Vector3 entitypos;
    Vector3 translation;
    Vector3 viseur;
    float *vm;//viewmatrix
    viseur = GetPlayerview(playerclass);
    while(1){
    vm = GetMatrix(matrixclass);
    entitypos.x = entity.xyz[0];
    entitypos.y = entity.xyz[1];
    entitypos.z = entity.xyz[2];  

    WorldToScreen(entitypos,aimscreenpos, vm, screenX, screenY);
    DrawLine(screenX/2,screenY/2, aimscreenpos.x, aimscreenpos.y);
    }

}
#endif