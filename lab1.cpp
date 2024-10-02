// lab1.cpp : Определяет точку входа для приложения.
//

#include <Windows.h>
#include <string>


#include "framework.h"
#include "lab1.h"

#define MAX_LOADSTRING 100

// Глобальная переменная для хранения пути к выбранному файлу
std::wstring selectedFilePath;

// Глобальные переменные:
HINSTANCE hInst;                                // текущий экземпляр
WCHAR szTitle[MAX_LOADSTRING];                  // Текст строки заголовка
WCHAR szWindowClass[MAX_LOADSTRING];            // имя класса главного окна

// Отправить объявления функций, включенных в этот модуль кода:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Разместите код здесь.

    // Инициализация глобальных строк
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_LAB1, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Выполнить инициализацию приложения:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_LAB1));

    MSG msg;

    // Цикл основного сообщения:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_LAB1));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDM_USERGUIDE); // Заменяем на новый идентификатор меню
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_LAB1));

    return RegisterClassExW(&wcex);
}


BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance; // Сохранить маркер экземпляра в глобальной переменной

    HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

    if (!hWnd)
    {
        return FALSE;
    }

    // Загружаем меню
    HMENU hMenu = LoadMenu(hInstance, MAKEINTRESOURCE(IDC_LAB1));
    SetMenu(hWnd, hMenu);

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    return TRUE;
}



// Функция для загрузки изображения .bmp
HBITMAP LoadBitmapFromFile(const wchar_t* filename) {
    return (HBITMAP)LoadImage(NULL, filename, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
}

//!
// Функция для открытия файла через проводник и возврата его полного пути
std::wstring OpenFile(HWND hWnd) {
    OPENFILENAME ofn;
    WCHAR szFile[MAX_PATH];

    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hWnd;
    ofn.lpstrFile = szFile;
    ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = L"Bitmap Files (*.bmp)\0*.bmp\0All Files (*.*)\0*.*\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrInitialDir = NULL;
    ofn.lpstrTitle = L"Select a Bitmap File";
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

    if (GetOpenFileName(&ofn) == TRUE) {
        return ofn.lpstrFile;
    }
    else {
        return L"";
    }
}

//! 


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        // Разобрать выбор в меню:
        switch (wmId)
        {
        case IDM_ABOUT:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
            break;
        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;
        case IDM_USERGUIDE:
            MessageBox(hWnd, L"Для вывода файла изображение в формате .bmp в верхнем меню перейдите во вкладку \"Файл\", затем нажмите \"Открыть\". В появившемся окне выберете необходимый файл.", L"Руководство пользователя", MB_OK | MB_ICONINFORMATION);
            break;
        case IDM_OPEN:
        {
            std::wstring filePath = OpenFile(hWnd);
            if (!filePath.empty()) {
                // Сохраняем путь к выбранному файлу
                selectedFilePath = filePath;
                // Перерисовываем окно для отображения нового изображения
                InvalidateRect(hWnd, NULL, TRUE);
            }
        }
        break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    break;
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);

        // Проверяем, есть ли выбранное изображение
        if (!selectedFilePath.empty()) {
            // Загружаем выбранное изображение
            HBITMAP hBitmap = LoadBitmapFromFile(selectedFilePath.c_str());
            if (hBitmap != NULL) {
                HDC hdcMem = CreateCompatibleDC(hdc);
                HBITMAP hBitmapOld = (HBITMAP)SelectObject(hdcMem, hBitmap);

                // Определить размеры изображения
                BITMAP bm;
                GetObject(hBitmap, sizeof(bm), &bm);

                // Определить размеры клиентской области окна
                RECT clientRect;
                GetClientRect(hWnd, &clientRect);
                int clientWidth = clientRect.right - clientRect.left;
                int clientHeight = clientRect.bottom - clientRect.top;

                // Рассчитать координаты для вывода изображения по центру окна
                int x = (clientWidth - bm.bmWidth) / 2;
                int y = (clientHeight - bm.bmHeight) / 2;

                // Отрисовать выбранное изображение
                BitBlt(hdc, x, y, bm.bmWidth, bm.bmHeight, hdcMem, 0, 0, SRCCOPY);

                // Очистить ресурсы
                SelectObject(hdcMem, hBitmapOld);
                DeleteDC(hdcMem);
                DeleteObject(hBitmap);
            }
            else {
                // В случае ошибки загрузки изображения, выводим сообщение об ошибке
                MessageBox(hWnd, L"Не удалось загрузить изображение. Убедитесь, что выбран правильный файл BMP.", L"Ошибка", MB_OK | MB_ICONERROR);
            }
        }

        EndPaint(hWnd, &ps);
    }
    break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Обработчик сообщений для окна "О программе".
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
