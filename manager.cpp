#include "manager.h"

Manager::Manager(MYSQL* mysql, char* request){
    m_mysql = mysql;
	stored_res = NULL;
	global_request = request;
}

Manager::~Manager() {
	mysql_free_result(stored_res);
}

void Manager::open(HWND parent) {
    DialogBoxParam(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_MANAGE_STUDENTS), parent, Manager::dlgProc, (LPARAM) this);
}

BOOL CALLBACK Manager::dlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) {
    switch(Message) {
        case WM_INITDIALOG:
        {
			Manager* mymng = (Manager*) lParam;
			SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG) mymng);
            mymng->updateList(hwnd, mymng->global_request);
			break;
        }
        case WM_COMMAND:
		{
			Manager* mymng = (Manager*) GetWindowLongPtr(hwnd, GWLP_USERDATA);
            switch(LOWORD(wParam)) {
                case ID_ADD:
                    mymng->addM(hwnd);
                    break;
                case ID_UPDATE:
                    mymng->updateM(hwnd);
                    break;
                case ID_DELETE:
                    mymng->deleteM(hwnd);
                    break;
				case ID_CLOSE:
					EndDialog(hwnd, 0);
					break;
                case IDC_LIST:
                    // It's our listbox, check the notification code
                    switch(HIWORD(wParam)) {
                        case LBN_DBLCLK:
                            mymng->updateM(hwnd);
                            break;
                    }
                    break;
            }
            break;
		}
        case WM_CLOSE:
			EndDialog(hwnd, 0);
			break;
        default:
            return FALSE;
    }
    return TRUE;
}

void Manager::updateList(HWND hwnd, const char* request) {
    HWND hList = GetDlgItem(hwnd, IDC_LIST);
    unsigned int i = 0;

    // Clear the list
    SendMessage(hList, LB_RESETCONTENT, 0, 0);

    // Make the request
    mysql_query(m_mysql, request);
    MYSQL_ROW row;
    stored_res = mysql_store_result(m_mysql);

    // Treat the results
    for(i=0; (row = mysql_fetch_row(stored_res)); i++) {
        int pos = SendDlgItemMessage(hwnd, IDC_LIST, LB_ADDSTRING, 0, (LPARAM) this->generateStrM(row));
        SendMessage(hList, LB_SETITEMDATA, pos, (LPARAM) this->setDataM(row));
    }
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
/*
ManagerStudents::ManagerStudents(){

}

ManagerStudents::~ManagerStudents() {

}


*/
