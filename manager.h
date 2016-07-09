#pragma once

#include <winsock.h> // For MySQL
#include <MYSQL/mysql.h> // For MySQL
#include <tchar.h>
#include <windows.h>
#include <string.h>
#include "resource.h"

class Manager {
    public:

    Manager(MYSQL* mysql, char* request);
    ~Manager();
    void open(HWND parent);
    static BOOL CALLBACK dlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam);
    //void fillDialogs();
	void updateList(HWND hwnd, const char* request);

    virtual int addM(HWND hwnd);
    virtual int updateM(HWND hwnd);
    virtual int deleteM(HWND hwnd);
    virtual char* generateStrM(const MYSQL_ROW);
    virtual void* setDataM(const MYSQL_ROW);

    private:
    MYSQL* m_mysql;
	MYSQL_RES* stored_res;
	char* global_request;

};
/*
class ManagerStudents : public Manager  {
    public:

    ManagerStudents();
    ~ManagerStudents();

    private:


};*/

