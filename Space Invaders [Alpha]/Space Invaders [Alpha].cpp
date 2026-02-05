// MADE BY DllLibStudio IN 6 HOURS
// GAME IS IN ALPHA AND THERE MAY BE BUGS !!
// IF YOU FOUN








#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>
#include <vector>
#include <string>
#include <ctime>
#include <cstdlib>
#include <sstream>

#pragma comment(linker, "/SUBSYSTEM:WINDOWS")
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")

// Константы игры
const int SCREEN_WIDTH = 600;
const int SCREEN_HEIGHT = 700;
const int PLAYER_SPEED = 10;
const int BULLET_SPEED = 12;
const int ENEMY_SPEED = 2;
const int ENEMY_ROWS = 6;
const int ENEMY_COLS = 12;
const int ENEMY_WIDTH = 45;
const int ENEMY_HEIGHT = 35;

// Цвета
const COLORREF COLOR_BLACK = RGB(0, 0, 0);
const COLORREF COLOR_BLUE = RGB(0, 0, 168);
const COLORREF COLOR_GREEN = RGB(0, 168, 0);
const COLORREF COLOR_CYAN = RGB(0, 168, 168);
const COLORREF COLOR_RED = RGB(168, 0, 0);
const COLORREF COLOR_MAGENTA = RGB(168, 0, 168);
const COLORREF COLOR_BROWN = RGB(168, 84, 0);
const COLORREF COLOR_LIGHTGRAY = RGB(168, 168, 168);
const COLORREF COLOR_DARKGRAY = RGB(84, 84, 84);
const COLORREF COLOR_LIGHTBLUE = RGB(84, 84, 255);
const COLORREF COLOR_LIGHTGREEN = RGB(84, 255, 84);
const COLORREF COLOR_LIGHTCYAN = RGB(84, 255, 255);
const COLORREF COLOR_LIGHTRED = RGB(255, 84, 84);
const COLORREF COLOR_LIGHTMAGENTA = RGB(255, 84, 255);
const COLORREF COLOR_YELLOW = RGB(255, 255, 0);
const COLORREF COLOR_WHITE = RGB(255, 255, 255);

// Структуры для игровых объектов
struct Bullet {
    int x, y;
    bool active;
    bool fromPlayer;
};

struct Enemy {
    int x, y;
    bool active;
    int type;
    int health;
};

struct Explosion {
    int x, y;
    int frame;
    int maxFrames;
};

// Глобальные переменные игры
HWND g_hWnd;
HDC g_hdcBuffer;
HBITMAP g_hbmBuffer;
RECT g_rcClient;

int playerX = SCREEN_WIDTH / 2;
int playerY = SCREEN_HEIGHT - 80;
int playerLives = 5;
int score = 0;
int level = 1;
bool gameRunning = true;
bool gamePaused = false;
bool leftPressed = false;
bool rightPressed = false;
bool spacePressed = false;

std::vector<Bullet> bullets;
std::vector<Enemy> enemies;
std::vector<Explosion> explosions;

// Функции рисования
void DrawTextA(HDC hdc, int x, int y, const char* text) {
    TextOutA(hdc, x, y, text, (int)strlen(text));
}

void DrawRect(HDC hdc, int x, int y, int w, int h, COLORREF color) {
    HBRUSH hBrush = CreateSolidBrush(color);
    HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);
    HPEN hPen = CreatePen(PS_SOLID, 1, color);
    HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);

    Rectangle(hdc, x, y, x + w, y + h);

    SelectObject(hdc, hOldBrush);
    SelectObject(hdc, hOldPen);
    DeleteObject(hBrush);
    DeleteObject(hPen);
}

void DrawPlayer(HDC hdc, int x, int y) {
    // Основной корпус
    DrawRect(hdc, x - 20, y, 40, 30, COLOR_LIGHTCYAN);
    DrawRect(hdc, x - 15, y - 15, 30, 15, COLOR_LIGHTCYAN);

    // Кабина
    DrawRect(hdc, x - 8, y - 12, 16, 12, COLOR_LIGHTBLUE);

    // Двигатели
    DrawRect(hdc, x - 16, y + 30, 8, 15, COLOR_LIGHTRED);
    DrawRect(hdc, x + 8, y + 30, 8, 15, COLOR_LIGHTRED);

    // Оружие
    DrawRect(hdc, x - 12, y - 8, 6, 8, COLOR_YELLOW);
    DrawRect(hdc, x + 6, y - 8, 6, 8, COLOR_YELLOW);

    // Защита
    DrawRect(hdc, x - 22, y + 5, 4, 20, COLOR_CYAN);
    DrawRect(hdc, x + 18, y + 5, 4, 20, COLOR_CYAN);
}

void DrawEnemy(HDC hdc, int x, int y, int type) {
    if (type == 0) {
        // Обычный враг
        DrawRect(hdc, x - 20, y, 40, 30, COLOR_LIGHTRED);
        DrawRect(hdc, x - 15, y - 12, 30, 12, COLOR_LIGHTRED);

        // Глаза
        DrawRect(hdc, x - 10, y - 6, 8, 6, COLOR_WHITE);
        DrawRect(hdc, x + 2, y - 6, 8, 6, COLOR_WHITE);
        DrawRect(hdc, x - 8, y - 3, 4, 3, COLOR_BLUE);
        DrawRect(hdc, x + 4, y - 3, 4, 3, COLOR_BLUE);

        // Рот
        DrawRect(hdc, x - 6, y + 5, 12, 4, COLOR_MAGENTA);
    }
    else {
        // Босс
        DrawRect(hdc, x - 25, y, 50, 35, COLOR_MAGENTA);
        DrawRect(hdc, x - 20, y - 15, 40, 15, COLOR_MAGENTA);

        // Щиты
        DrawRect(hdc, x - 23, y + 8, 6, 20, COLOR_CYAN);
        DrawRect(hdc, x + 17, y + 8, 6, 20, COLOR_CYAN);

        // Оружие
        DrawRect(hdc, x - 12, y - 8, 8, 8, COLOR_YELLOW);
        DrawRect(hdc, x + 4, y - 8, 8, 8, COLOR_YELLOW);

        // Антенна
        DrawRect(hdc, x - 2, y - 20, 4, 5, COLOR_LIGHTGREEN);
    }
}

void DrawBullet(HDC hdc, int x, int y, bool fromPlayer) {
    if (fromPlayer) {
        // Пуля игрока
        DrawRect(hdc, x - 3, y - 8, 6, 16, COLOR_LIGHTGREEN);
        DrawRect(hdc, x - 2, y - 10, 4, 2, COLOR_YELLOW);
    }
    else {
        // Пуля врага
        DrawRect(hdc, x - 3, y - 8, 6, 16, COLOR_LIGHTRED);
        DrawRect(hdc, x - 2, y - 10, 4, 2, COLOR_MAGENTA);
    }
}

void DrawExplosion(HDC hdc, int x, int y, int frame) {
    int radius = frame * 4;
    COLORREF colors[] = { COLOR_YELLOW, COLOR_LIGHTRED, COLOR_RED, COLOR_BROWN };
    COLORREF color = colors[frame % 4];

    for (int i = 0; i < 8; i++) {
        int dx = (int)(cos(i * 3.14159 / 4) * radius);
        int dy = (int)(sin(i * 3.14159 / 4) * radius);
        DrawRect(hdc, x + dx - 3, y + dy - 3, 6, 6, color);
    }

    // Центр взрыва
    DrawRect(hdc, x - 4, y - 4, 8, 8, COLOR_WHITE);
}

// Инициализация игры
void InitGame() {
    bullets.clear();
    enemies.clear();
    explosions.clear();

    playerX = SCREEN_WIDTH / 2;
    playerLives = 5;
    score = 0;
    level = 1;
    gameRunning = true;

    // Создаем врагов
    for (int row = 0; row < ENEMY_ROWS; row++) {
        for (int col = 0; col < ENEMY_COLS; col++) {
            Enemy enemy;
            enemy.x = 120 + col * (ENEMY_WIDTH + 30);
            enemy.y = 70 + row * (ENEMY_HEIGHT + 30);
            enemy.active = true;
            enemy.type = (row < 2) ? 1 : 0;  // Первые два ряда - боссы
            enemy.health = enemy.type == 1 ? 3 : 1;
            enemies.push_back(enemy);
        }
    }
}

// Обработка ввода
void ProcessInput() {
    if (!gameRunning || gamePaused) return;

    if (leftPressed && playerX > 40) {
        playerX -= PLAYER_SPEED;
    }
    if (rightPressed && playerX < SCREEN_WIDTH - 40) {
        playerX += PLAYER_SPEED;
    }

    static bool canShoot = true;
    if (spacePressed && canShoot) {
        Bullet bullet;
        bullet.x = playerX;
        bullet.y = playerY - 25;
        bullet.active = true;
        bullet.fromPlayer = true;
        bullets.push_back(bullet);

        canShoot = false;
        SetTimer(g_hWnd, 1, 250, NULL);
    }
}

// Обновление игровой логики
void UpdateGame() {
    if (!gameRunning || gamePaused) return;

    // Движение пуль
    for (auto& bullet : bullets) {
        if (bullet.active && bullet.fromPlayer) {
            bullet.y -= BULLET_SPEED;
            if (bullet.y < 0) bullet.active = false;
        }
        else if (bullet.active && !bullet.fromPlayer) {
            bullet.y += BULLET_SPEED;
            if (bullet.y > SCREEN_HEIGHT) bullet.active = false;
        }
    }

    // Движение врагов
    static int enemyDirection = 1;
    static int enemyMoveTimer = 0;

    enemyMoveTimer++;
    if (enemyMoveTimer > 25 - level * 2) {
        enemyMoveTimer = 0;

        // Проверяем, не достигли ли края
        bool changeDirection = false;
        for (const auto& enemy : enemies) {
            if (enemy.active) {
                if ((enemy.x < 60 && enemyDirection == -1) ||
                    (enemy.x > SCREEN_WIDTH - 60 && enemyDirection == 1)) {
                    changeDirection = true;
                    break;
                }
            }
        }

        // Двигаем врагов
        for (auto& enemy : enemies) {
            if (enemy.active) {
                enemy.x += ENEMY_SPEED * enemyDirection;

                // Случайный выстрел врага
                if (rand() % 150 < 2) {
                    Bullet bullet;
                    bullet.x = enemy.x;
                    bullet.y = enemy.y + 25;
                    bullet.active = true;
                    bullet.fromPlayer = false;
                    bullets.push_back(bullet);
                }
            }
        }

        // Меняем направление если нужно
        if (changeDirection) {
            enemyDirection *= -1;
            for (auto& enemy : enemies) {
                if (enemy.active) {
                    enemy.y += 25;
                    // Если враги достигли низа - игра окончена
                    if (enemy.y > SCREEN_HEIGHT - 120) {
                        gameRunning = false;
                    }
                }
            }
        }
    }

    // Проверка столкновений пуль игрока с врагами
    for (auto& bullet : bullets) {
        if (bullet.active && bullet.fromPlayer) {
            for (auto& enemy : enemies) {
                if (enemy.active) {
                    if (bullet.x > enemy.x - ENEMY_WIDTH / 2 &&
                        bullet.x < enemy.x + ENEMY_WIDTH / 2 &&
                        bullet.y > enemy.y - ENEMY_HEIGHT / 2 &&
                        bullet.y < enemy.y + ENEMY_HEIGHT / 2) {

                        bullet.active = false;
                        enemy.health--;

                        if (enemy.health <= 0) {
                            enemy.active = false;
                            score += enemy.type == 1 ? 150 : 75;

                            // Создаем взрыв
                            Explosion explosion;
                            explosion.x = enemy.x;
                            explosion.y = enemy.y;
                            explosion.frame = 0;
                            explosion.maxFrames = 8;
                            explosions.push_back(explosion);
                        }
                        break;
                    }
                }
            }
        }
    }

    // Проверка столкновений пуль врагов с игроком
    for (auto& bullet : bullets) {
        if (bullet.active && !bullet.fromPlayer) {
            if (bullet.x > playerX - 25 &&
                bullet.x < playerX + 25 &&
                bullet.y > playerY - 25 &&
                bullet.y < playerY + 35) {

                bullet.active = false;
                playerLives--;

                // Создаем взрыв
                Explosion explosion;
                explosion.x = playerX;
                explosion.y = playerY;
                explosion.frame = 0;
                explosion.maxFrames = 8;
                explosions.push_back(explosion);

                if (playerLives <= 0) {
                    gameRunning = false;
                }
                break;
            }
        }
    }

    // Обновление анимаций взрывов
    for (auto it = explosions.begin(); it != explosions.end(); ) {
        it->frame++;
        if (it->frame >= it->maxFrames) {
            it = explosions.erase(it);
        }
        else {
            ++it;
        }
    }

    // Проверка на завершение уровня
    bool enemiesLeft = false;
    for (const auto& enemy : enemies) {
        if (enemy.active) {
            enemiesLeft = true;
            break;
        }
    }

    if (!enemiesLeft) {
        level++;
        InitGame();
    }
}

// Отрисовка игры
void RenderGame(HDC hdc) {
    // Очищаем экран
    DrawRect(hdc, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, COLOR_BLACK);

    // Рисуем звездное небо
    for (int i = 0; i < 150; i++) {
        int x = rand() % SCREEN_WIDTH;
        int y = rand() % SCREEN_HEIGHT;
        int size = rand() % 3 + 1;
        DrawRect(hdc, x, y, size, size, COLOR_WHITE);
    }

    // Рисуем игрока
    if (gameRunning) {
        DrawPlayer(hdc, playerX, playerY);
    }

    // Рисуем врагов
    for (const auto& enemy : enemies) {
        if (enemy.active) {
            DrawEnemy(hdc, enemy.x, enemy.y, enemy.type);
        }
    }

    // Рисуем пули
    for (const auto& bullet : bullets) {
        if (bullet.active) {
            DrawBullet(hdc, bullet.x, bullet.y, bullet.fromPlayer);
        }
    }

    // Рисуем взрывы
    for (const auto& explosion : explosions) {
        DrawExplosion(hdc, explosion.x, explosion.y, explosion.frame);
    }

    // Рисуем интерфейс
    char buffer[256];

    // Верхняя панель
    DrawRect(hdc, 0, 0, SCREEN_WIDTH, 35, COLOR_DARKGRAY);
    SetBkColor(hdc, COLOR_DARKGRAY);
    SetTextColor(hdc, COLOR_YELLOW);

    sprintf_s(buffer, "SCORE: %08d", score);
    DrawTextA(hdc, 20, 10, buffer);

    sprintf_s(buffer, "LEVEL: %02d", level);
    DrawTextA(hdc, 250, 10, buffer);

    sprintf_s(buffer, "LIVES: %d", playerLives);
    DrawTextA(hdc, 400, 10, buffer);

    // Индикатор жизней
    for (int i = 0; i < playerLives; i++) {
        DrawRect(hdc, 480 + i * 25, 12, 6, 15, COLOR_LIGHTRED);
        DrawRect(hdc, 480 + i * 25, 8, 6, 4, COLOR_LIGHTCYAN);
    }

    // Управление
    SetTextColor(hdc, COLOR_LIGHTGREEN);
    DrawTextA(hdc, 600, 10, "ARROWS: MOVE | SPACE: FIRE | P: PAUSE | R: RESTART | ESC: EXIT");

    // Сообщения
    if (!gameRunning) {
        DrawRect(hdc, SCREEN_WIDTH / 2 - 150, SCREEN_HEIGHT / 2 - 50, 300, 100, COLOR_BLUE);
        DrawRect(hdc, SCREEN_WIDTH / 2 - 145, SCREEN_HEIGHT / 2 - 45, 290, 90, COLOR_LIGHTBLUE);

        SetBkColor(hdc, COLOR_LIGHTBLUE);
        SetTextColor(hdc, COLOR_YELLOW);

        if (playerLives <= 0) {
            DrawTextA(hdc, SCREEN_WIDTH / 2 - 50, SCREEN_HEIGHT / 2 - 20, "GAME OVER!");
        }
        else {
            DrawTextA(hdc, SCREEN_WIDTH / 2 - 40, SCREEN_HEIGHT / 2 - 20, "YOU WIN!");
        }

        sprintf_s(buffer, "FINAL SCORE: %d", score);
        DrawTextA(hdc, SCREEN_WIDTH / 2 - 60, SCREEN_HEIGHT / 2 + 0, buffer);

        DrawTextA(hdc, SCREEN_WIDTH / 2 - 90, SCREEN_HEIGHT / 2 + 20, "PRESS R TO PLAY AGAIN");
    }

    if (gamePaused) {
        DrawRect(hdc, SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 - 30, 200, 60, COLOR_DARKGRAY);
        DrawRect(hdc, SCREEN_WIDTH / 2 - 95, SCREEN_HEIGHT / 2 - 25, 190, 50, COLOR_BLACK);

        SetBkColor(hdc, COLOR_BLACK);
        SetTextColor(hdc, COLOR_YELLOW);

        DrawTextA(hdc, SCREEN_WIDTH / 2 - 30, SCREEN_HEIGHT / 2 - 10, "PAUSED");
        DrawTextA(hdc, SCREEN_WIDTH / 2 - 110, SCREEN_HEIGHT / 2 + 10, "PRESS P TO CONTINUE");
    }
}

// Оконная процедура
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_CREATE:
        g_hWnd = hWnd;
        srand((unsigned int)time(NULL));

        GetClientRect(hWnd, &g_rcClient);
        HDC hdc = GetDC(hWnd);
        g_hdcBuffer = CreateCompatibleDC(hdc);
        g_hbmBuffer = CreateCompatibleBitmap(hdc, g_rcClient.right, g_rcClient.bottom);
        SelectObject(g_hdcBuffer, g_hbmBuffer);
        ReleaseDC(hWnd, hdc);

        InitGame();
        SetTimer(hWnd, 2, 16, NULL);
        break;

    case WM_KEYDOWN:
        switch (wParam) {
        case VK_LEFT: leftPressed = true; break;
        case VK_RIGHT: rightPressed = true; break;
        case VK_SPACE: spacePressed = true; break;
        case 'P': case 'p': gamePaused = !gamePaused; break;
        case 'R': case 'r': InitGame(); break;
        case VK_ESCAPE: PostQuitMessage(0); break;
        }
        break;

    case WM_KEYUP:
        switch (wParam) {
        case VK_LEFT: leftPressed = false; break;
        case VK_RIGHT: rightPressed = false; break;
        case VK_SPACE: spacePressed = false; break;
        }
        break;

    case WM_TIMER:
        if (wParam == 1) {
            KillTimer(hWnd, 1);
        }
        else if (wParam == 2) {
            ProcessInput();
            UpdateGame();
            InvalidateRect(hWnd, NULL, FALSE);
        }
        break;

    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);

        RenderGame(g_hdcBuffer);
        BitBlt(hdc, 0, 0, g_rcClient.right, g_rcClient.bottom,
            g_hdcBuffer, 0, 0, SRCCOPY);

        EndPaint(hWnd, &ps);
        break;
    }

    case WM_ERASEBKGND:
        return 1;

    case WM_DESTROY:
        KillTimer(hWnd, 1);
        KillTimer(hWnd, 2);
        DeleteDC(g_hdcBuffer);
        DeleteObject(g_hbmBuffer);
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hWnd, msg, wParam, lParam);
    }

    return 0;
}

// Главная функция
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    // Регистрация класса окна
    WNDCLASSEXA wcex;
    ZeroMemory(&wcex, sizeof(wcex));
    wcex.cbSize = sizeof(WNDCLASSEXA);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.hInstance = hInstance;
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_BLACK);
    wcex.lpszClassName = "SpaceGameClass";
    wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);

    if (!RegisterClassExA(&wcex)) {
        return 1;
    }

    // Создание окна
    HWND hWnd = CreateWindowExA(
        0,
        "SpaceGameClass",
        "Space Defender - 90s Game",
        WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX & ~WS_THICKFRAME,
        CW_USEDEFAULT, CW_USEDEFAULT,
        SCREEN_WIDTH + 16, SCREEN_HEIGHT + 39,
        NULL, NULL, hInstance, NULL
    );

    if (!hWnd) {
        return 1;
    }

    // Показ окна
    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    // Цикл сообщений
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}   