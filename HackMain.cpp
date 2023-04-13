#ifndef HACKMAIN_CPP
#define HACKMAIN_CPP
#include "ESP.cpp"
#include "aim.cpp"
int main(){
    setupWindowHandles(L"AssaultCube");
    GetSWindowsSize();
    if(pid == -1){
        printf("Erreur, le logiciel n'est pas reconnu");
        return 1;
    }
    if (window)
    {
        GetWindowThreadProcessId(window, &pid);
        if (pid != 0)
        {
            windowHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);

            if (windowHandle == INVALID_HANDLE_VALUE || windowHandle == NULL || modulebase == 0)
            {
                printf("No handle");
                return 1;
            }
            else
            {
                printf("OK\n");
            }
        }
        else
        {
            printf("No access");
            return 1;
        }
    }
    else
    {
        printf("No window");
        return 1;

    }
    //REDACTED
}
#endif


/*
COMPILATION:
//REDACTED
*/
