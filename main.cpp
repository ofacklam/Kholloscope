/*#if defined(UNICODE) && !defined(_UNICODE)
    #define _UNICODE
#elif defined(_UNICODE) && !defined(UNICODE)
    #define UNICODE
#endif*/

#include <winsock.h> // For MySQL
#include <MYSQL/mysql.h> // For MySQL
#include <tchar.h>
#include <windows.h>
#include <string.h>

MYSQL* mysql = NULL;

#include "utilitaire.h"
#include "resource.h"
#include "manager2.h"


/*
typedef struct Link {
    unsigned int id_list;
    void* elem;
} Link;

typedef struct Links {
    Link* lst;
    int taille;
} Links;
*/

/*  Declare Windows procedure  */
LRESULT CALLBACK WindowProcedure (HWND, UINT, WPARAM, LPARAM);

/*  Make the class name into a global variable  */
TCHAR szClassName[ ] = _T("CodeBlocksWindowsApp");

int init_mysql(){
    mysql = mysql_init(NULL);
    if(mysql==NULL) {
        exit(42);
    }
    mysql_options(mysql,MYSQL_READ_DEFAULT_GROUP,"option");

    if(!mysql_real_connect(mysql,"localhost","root","","lataupe", 0, NULL, 0)) {
        exit(42);
    }
    return 0;
}

int WINAPI WinMain (HINSTANCE hThisInstance, HINSTANCE hPrevInstance, LPSTR lpszArgument, int nCmdShow) {
    HWND hwnd;               /* This is the handle for our window */
    MSG messages;            /* Here messages to the application are saved */
    WNDCLASSEX wincl;        /* Data structure for the windowclass */

    /* The Window structure */
    wincl.hInstance = hThisInstance;
    wincl.lpszClassName = szClassName;
    wincl.lpfnWndProc = WindowProcedure;      /* This function is called by windows */
    wincl.style = CS_DBLCLKS;                 /* Catch double-clicks */
    wincl.cbSize = sizeof (WNDCLASSEX);

    /* Use default icon and mouse-pointer */
    wincl.hIcon = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hIconSm = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hCursor = LoadCursor (NULL, IDC_ARROW);
    wincl.lpszMenuName = MAKEINTRESOURCE(IDR_MYMENU);                 /* Menu */
    wincl.cbClsExtra = 0;                      /* No extra bytes after the window class */
    wincl.cbWndExtra = 0;                      /* structure or the window instance */
    /* Use Windows's default colour as the background of the window */
    wincl.hbrBackground = (HBRUSH) COLOR_BACKGROUND;

    /* Register the window class, and if it fails quit the program */
    if(!RegisterClassEx (&wincl))
        return 0;

    /* The class is registered, let's create the program*/
    hwnd = CreateWindowEx (
           0,                   /* Extended possibilites for variation */
           szClassName,         /* Classname */
           _T("Kholloscope"),       /* Title Text */
           WS_OVERLAPPEDWINDOW, /* default window */
           CW_USEDEFAULT,       /* Windows decides the position */
           CW_USEDEFAULT,       /* where the window ends up on the screen */
           1000,                 /* The programs width */
           500,                 /* and height in pixels */
           HWND_DESKTOP,        /* The window is a child-window to desktop */
           NULL,                /* No menu */
           hThisInstance,       /* Program Instance handler */
           NULL                 /* No Window Creation data */
           );

    /* Make the window visible on the screen */
    ShowWindow(hwnd, nCmdShow);


    /*======== MYSQL ========*/
    init_mysql();


    /* Run the message loop. It will run until GetMessage() returns 0 */
    while (GetMessage (&messages, NULL, 0, 0))
    {
        /* Translate virtual-key messages into character messages */
        TranslateMessage(&messages);
        /* Send message to WindowProcedure */
        DispatchMessage(&messages);
    }

    /* The program return-value is 0 - The value that PostQuitMessage() gave */
    return messages.wParam;
}


/*  This function is called by the Windows function DispatchMessage()  */
LRESULT CALLBACK WindowProcedure (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)                  /* handle the messages */
    {
        case WM_COMMAND:
            switch(LOWORD(wParam)) {
                case ID_FILE_EXIT:
                    PostQuitMessage (0);
                    break;
                case ID_DB_STUDENTS:
                    DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_MANAGE_STUDENTS), hwnd, DB_ManagerStudents_Proc);
                    break;
                case ID_DB_GROUPS:
                    DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_MANAGE_GROUPS), hwnd, DB_ManagerGroups_Proc);
                    break;
                case ID_DB_SUBJECTS:
                    DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_MANAGE_SUBJECTS), hwnd, DB_ManagerSubjects_Proc);
                    break;
                case ID_DB_KHOLLEURS:
                    DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_MANAGE_KHOLLEURS), hwnd, DB_ManagerKholleurs_Proc);
                    break;
            }
            break;
	    case WM_RBUTTONDBLCLK:
            PostQuitMessage (0);
            break;
		case WM_CLOSE:
			//DestroyWindow(hwnd);
			PostQuitMessage (0);
			break;
        case WM_DESTROY:
            PostQuitMessage (0);       /* send a WM_QUIT to the message queue */
            break;
        default:                      /* for messages that we don't deal with */
            return DefWindowProc (hwnd, message, wParam, lParam);
    }

    return 0;
}

