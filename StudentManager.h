#pragma once
#include "manager.h"
class StudentManager :
	public Manager
{
public:
	StudentManager(MYSQL* mysql, char* request);
	~StudentManager(void);
	int addM(HWND hwnd);
    int updateM(HWND hwnd);
    int deleteM(HWND hwnd);
    char* generateStrM(const MYSQL_ROW);
    void* setDataM(const MYSQL_ROW);
};

