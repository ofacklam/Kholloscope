#include "StudentManager.h"


StudentManager::StudentManager(MYSQL* mysql, char* request) : Manager(mysql, request)
{
}


StudentManager::~StudentManager(void)
{
}

int Manager::addM(HWND hwnd) {
    return 0;
}
int Manager::updateM(HWND hwnd) {
    return 0;
}
int Manager::deleteM(HWND hwnd) {
    return 0;
}

char* Manager::generateStrM(const MYSQL_ROW) {
	return "";
}

void* Manager::setDataM(const MYSQL_ROW) {
	return NULL;
}
