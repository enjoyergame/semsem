#include "ui.h"

static HWND g_hwnd = NULL; 
static HDC g_hbc = NULL; 
static HBITMAP g_hbm = NULL, g_old = NULL;

static HDC g_hdt[6] = {0}; 
static HBITMAP g_hbt[6] = {0}, g_ot[6] = {0};
static int g_tw[6] = {0}, g_th[6] = {0};

static HDC g_hdw = NULL; 
static HBITMAP g_hbw = NULL, g_ow = NULL; 
static int g_ww = 0, g_wh = 0;

static HDC g_hds[7] = {0}; 
static HBITMAP g_hbs[7] = {0}, g_os[7] = {0};
static int g_sw[7] = {0}, g_sh[7] = {0};

static HDC g_hdcFaces[4] = {0}; 
static HBITMAP g_hbmFaces[4] = {0}, g_hbmOldFaces[4] = {0};
static int g_faceWidth[4] = {0}, g_faceHeight[4] = {0};

static bool g_fullscreen = false;
static int g_screenWidth = 800;  
static int g_screenHeight = 600; 

void UI_ToggleFullscreen() {
    static WINDOWPLACEMENT wpPrev = { sizeof(wpPrev) };
    DWORD dwStyle = GetWindowLong(g_hwnd, GWL_STYLE);
    
    if (!g_fullscreen) {
        MONITORINFO mi = { sizeof(mi) };
        if (GetWindowPlacement(g_hwnd, &wpPrev) && GetMonitorInfo(MonitorFromWindow(g_hwnd, MONITOR_DEFAULTTOPRIMARY), &mi)) {
            SetWindowLong(g_hwnd, GWL_STYLE, dwStyle & ~WS_OVERLAPPEDWINDOW);
            SetWindowPos(g_hwnd, HWND_TOP, mi.rcMonitor.left, mi.rcMonitor.top, mi.rcMonitor.right - mi.rcMonitor.left, mi.rcMonitor.bottom - mi.rcMonitor.top, SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
            g_screenWidth = mi.rcMonitor.right - mi.rcMonitor.left; 
            g_screenHeight = mi.rcMonitor.bottom - mi.rcMonitor.top; 
            ShowCursor(FALSE);
        }
    } else {
        SetWindowLong(g_hwnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW); 
        SetWindowPlacement(g_hwnd, &wpPrev);
        SetWindowPos(g_hwnd, NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
        g_screenWidth = 800; 
        g_screenHeight = 600; 
        ShowCursor(TRUE);
    }
    g_fullscreen = !g_fullscreen;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    if (uMsg == WM_DESTROY) { 
        PostQuitMessage(0); 
        return 0; 
    }
    if (uMsg == WM_ERASEBKGND) {
        return 1; 
    }
    if (uMsg == WM_KEYDOWN && wParam == VK_F11) { 
        UI_ToggleFullscreen(); 
    } 
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

bool UI_Init(void) {
    WNDCLASS wc = {0}; 
    wc.lpfnWndProc = WindowProc; 
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = "DoomClass"; 
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    
    if (!RegisterClass(&wc)) return false;
    
    DWORD style = WS_OVERLAPPEDWINDOW;
    g_hwnd = CreateWindowEx(0, wc.lpszClassName, "Furry Porn (Press F11 for Fullscreen)", style, CW_USEDEFAULT, CW_USEDEFAULT, 800, 600, NULL, NULL, wc.hInstance, NULL);
    if (!g_hwnd) return false;
    
    RECT r = {0, 0, 800, 600}; 
    AdjustWindowRect(&r, style, FALSE);
    SetWindowPos(g_hwnd, NULL, 0, 0, r.right - r.left, r.bottom - r.top, SWP_NOMOVE | SWP_NOZORDER);
    
    HDC hdc = GetDC(g_hwnd);
    g_hbc = CreateCompatibleDC(hdc); 
    g_hbm = CreateCompatibleBitmap(hdc, 800, 600); 
    g_old = (HBITMAP)SelectObject(g_hbc, g_hbm);
    
    // Загрузка стен
    for (int i = 1; i <= 5; i++) {
        char p[64]; 
        snprintf(p, sizeof(p), "assets/wall%d.bmp", i);
        g_hbt[i] = (HBITMAP)LoadImageA(NULL, p, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
        if (g_hbt[i]) { 
            BITMAP bm; 
            GetObject(g_hbt[i], sizeof(BITMAP), &bm); 
            g_tw[i] = bm.bmWidth; 
            g_th[i] = bm.bmHeight; 
            g_hdt[i] = CreateCompatibleDC(hdc); 
            g_ot[i] = (HBITMAP)SelectObject(g_hdt[i], g_hbt[i]); 
        }
    }
    
    // Загрузка оружия
    g_hbw = (HBITMAP)LoadImageA(NULL, "assets/weapon.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
    if (g_hbw) { 
        BITMAP bm; 
        GetObject(g_hbw, sizeof(BITMAP), &bm); 
        g_ww = bm.bmWidth; 
        g_wh = bm.bmHeight; 
        g_hdw = CreateCompatibleDC(hdc); 
        g_ow = (HBITMAP)SelectObject(g_hdw, g_hbw); 
    }
    
    // Загрузка спрайтов
    const char* sf[7] = {
        "assets/spr_melee.bmp", 
        "assets/spr_ranged.bmp", 
        "assets/spr_fireball.bmp", 
        "assets/spr_medkit.bmp", 
        "assets/spr_ammo.bmp", 
        "assets/spr_player1.bmp", 
        "assets/spr_player2.bmp"
    };
    for (int i = 0; i < 7; i++) {
        g_hbs[i] = (HBITMAP)LoadImageA(NULL, sf[i], IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
        if (g_hbs[i]) { 
            BITMAP bm; 
            GetObject(g_hbs[i], sizeof(BITMAP), &bm); 
            g_sw[i] = bm.bmWidth; 
            g_sh[i] = bm.bmHeight; 
            g_hds[i] = CreateCompatibleDC(hdc); 
            g_os[i] = (HBITMAP)SelectObject(g_hds[i], g_hbs[i]); 
        }
    }
    
    // Загрузка лиц
    const char* ff[4] = {
        "assets/face1.bmp", 
        "assets/face2.bmp", 
        "assets/face3.bmp", 
        "assets/face4.bmp"
    };
    for (int i = 0; i < 4; i++) {
        g_hbmFaces[i] = (HBITMAP)LoadImageA(NULL, ff[i], IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
        if (g_hbmFaces[i]) { 
            BITMAP bm; 
            GetObject(g_hbmFaces[i], sizeof(BITMAP), &bm); 
            g_faceWidth[i] = bm.bmWidth; 
            g_faceHeight[i] = bm.bmHeight; 
            g_hdcFaces[i] = CreateCompatibleDC(hdc); 
            g_hbmOldFaces[i] = (HBITMAP)SelectObject(g_hdcFaces[i], g_hbmFaces[i]); 
        }
    }
    
    ReleaseDC(g_hwnd, hdc); 
    ShowWindow(g_hwnd, SW_SHOW); 
    return true;
}

bool UI_ProcessMessages(void) { 
    MSG msg; 
    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) { 
        if (msg.message == WM_QUIT) return false; 
        TranslateMessage(&msg); 
        DispatchMessage(&msg); 
    } 
    return true; 
}

void UI_UpdateInput(InputState* input) {
    input->w = (GetAsyncKeyState('W') & 0x8000); 
    input->s = (GetAsyncKeyState('S') & 0x8000);
    input->a = (GetAsyncKeyState('A') & 0x8000); 
    input->d = (GetAsyncKeyState('D') & 0x8000);
    input->space = (GetAsyncKeyState(VK_SPACE) & 0x8000);
    
    input->up = (GetAsyncKeyState(VK_UP) & 0x8000); 
    input->down = (GetAsyncKeyState(VK_DOWN) & 0x8000);
    input->left = (GetAsyncKeyState(VK_LEFT) & 0x8000); 
    input->right = (GetAsyncKeyState(VK_RIGHT) & 0x8000);
    input->enter = (GetAsyncKeyState(VK_RETURN) & 0x8000);
    
    input->reset = (GetAsyncKeyState('R') & 0x8000);
    input->k1 = (GetAsyncKeyState('1') & 0x8000); 
    input->k2 = (GetAsyncKeyState('2') & 0x8000);
}

void UI_Render(const GameState* state) {
    if (state->mode == MODE_MENU) {
        RECT r = {0, 0, 800, 600}; 
        HBRUSH b = CreateSolidBrush(RGB(30, 0, 0)); 
        FillRect(g_hbc, &r, b); 
        DeleteObject(b);
        
        SetBkMode(g_hbc, TRANSPARENT);
        HFONT hT = CreateFontA(72, 0, 0, 0, FW_BOLD, 0, 0, 0, 0, 0, 0, 0, 0, "Impact"); 
        SelectObject(g_hbc, hT); 
        SetTextColor(g_hbc, RGB(255, 0, 0)); 
        TextOutA(g_hbc, 230, 150, "Furry Porn", 10); 
        DeleteObject(hT);
        
        HFONT hM = CreateFontA(36, 0, 0, 0, FW_NORMAL, 0, 0, 0, 0, 0, 0, 0, 0, "Impact"); 
        SelectObject(g_hbc, hM); 
        SetTextColor(g_hbc, RGB(255, 255, 0)); 
        TextOutA(g_hbc, 270, 300, "Press [1] SOLO", 14); 
        TextOutA(g_hbc, 270, 370, "Press [2] CO-OP", 15); 
        DeleteObject(hM);
    } else {
        int num_views = (state->mode == MODE_GAME_COOP) ? 2 : 1;
        
        for (int p = 0; p < num_views; p++) {
            int start_y = (state->mode == MODE_GAME_SOLO) ? 0 : (p == 0 ? 0 : 252);
            int end_y   = (state->mode == MODE_GAME_SOLO) ? 500 : (p == 0 ? 248 : 500);
            int view_h  = end_y - start_y;

            if (!state->players[p].alive) {
                RECT blackRect = {0, start_y, 800, end_y};
                HBRUSH blackBrush = CreateSolidBrush(RGB(0, 0, 0));
                FillRect(g_hbc, &blackRect, blackBrush);
                DeleteObject(blackBrush);
                continue; 
            }

            HRGN clipRgn = CreateRectRgn(0, start_y, 800, end_y); 
            SelectClipRgn(g_hbc, clipRgn);
            
            RECT cr = {0, start_y, 800, start_y + view_h/2}; 
            HBRUSH cb = CreateSolidBrush(RGB(30, 30, 30)); 
            FillRect(g_hbc, &cr, cb); 
            DeleteObject(cb);
            
            RECT fr = {0, start_y + view_h/2, 800, end_y}; 
            HBRUSH fb = CreateSolidBrush(RGB(60, 60, 60)); 
            FillRect(g_hbc, &fr, fb); 
            DeleteObject(fb);
            
            // Отрисовка стен
            for (int x = 0; x < 800; x++) {
                int l = state->views[p].render_buffer[x].wall_height * view_h / 600.0f;
                int t = state->views[p].render_buffer[x].hit_type; 
                if (t < 1 || t > 5) t = 1;
                
                int dS = -l/2 + view_h/2 + start_y;
                int dE = l/2 + view_h/2 + start_y;
                int yS = dS < start_y ? start_y : dS;
                int yE = dE >= end_y ? end_y - 1 : dE;
                
                if (yE - yS > 0 && g_hbt[t]) {
                    int tx = (int)(state->views[p].render_buffer[x].texture_x * g_tw[t]);
                    int ty = dS < start_y ? (int)(-(dS - start_y) * ((float)g_th[t] / l)) : 0;
                    int sh = dS < start_y ? (int)((yE - yS) * ((float)g_th[t] / l)) : g_th[t];
                    StretchBlt(g_hbc, x, yS, 1, yE - yS, g_hdt[t], tx, ty, 1, sh <= 0 ? 1 : sh, SRCCOPY);
                }
            }
            
            // Отрисовка спрайтов
            for(int i = 0; i < state->views[p].num_sprites; i++) {
                RenderSprite s = state->views[p].render_sprites[i]; 
                float sx = s.x - state->players[p].x;
                float sy = s.y - state->players[p].y;
                float id = 1.0f / (state->players[p].plane_x * state->players[p].dir_y - state->players[p].dir_x * state->players[p].plane_y);
                float tx = id * (state->players[p].dir_y * sx - state->players[p].dir_x * sy);
                float ty = id * (-state->players[p].plane_y * sx + state->players[p].plane_x * sy);
                
                if (ty > 0.1f) {
                    int ssx = (int)(400 * (1 + tx / ty));
                    int sh = abs((int)(view_h / ty));
                    int dsy = -sh/2 + view_h/2 + start_y;
                    int sw = abs((int)(view_h / ty));
                    int dsx = -sw/2 + ssx;
                    int dex = sw/2 + ssx;
                    
                    for (int st = dsx; st < dex; st++) {
                        if (st > 0 && st < 800 && ty < state->views[p].z_buffer[st] && g_hbs[s.tex_id]) {
                            int tx_s = (int)(256 * (st - dsx) * g_sw[s.tex_id] / sw) / 256;
                            TransparentBlt(g_hbc, st, dsy, 1, sh, g_hds[s.tex_id], tx_s, 0, 1, g_sh[s.tex_id], RGB(255,0,255));
                        }
                    }
                }
            }

            // Отрисовка оружия
            if (g_hbw && !state->has_won) {
                int by = 0, bx = 0; 
                if (p == 0 && (GetAsyncKeyState('W') || GetAsyncKeyState('S') || GetAsyncKeyState('A') || GetAsyncKeyState('D'))) { 
                    by = (int)(sin(GetTickCount() / 150.0f) * 15.0f); 
                    bx = (int)(cos(GetTickCount() / 300.0f) * 10.0f); 
                }
                if (p == 1 && (GetAsyncKeyState(VK_UP) || GetAsyncKeyState(VK_DOWN) || GetAsyncKeyState(VK_LEFT) || GetAsyncKeyState(VK_RIGHT))) { 
                    by = (int)(sin(GetTickCount() / 150.0f) * 15.0f); 
                    bx = (int)(cos(GetTickCount() / 300.0f) * 10.0f); 
                }
                if (state->players[p].weapon_anim_time > 0) {
                    by += 30;
                }
                
                int dw = g_ww * 3;
                int dh = g_wh * 3;
                int px = 400 - (dw / 2) + bx;
                int py = end_y - dh + by;
                
                if (state->players[p].weapon_anim_time > 0.1f) {
                    HBRUSH f = CreateSolidBrush(RGB(255, 180, 0)); 
                    SelectObject(g_hbc, f); 
                    Ellipse(g_hbc, px + dw/2 - 20, py - 10, px + dw/2 + 20, py + 20); 
                    DeleteObject(f);
                }
                TransparentBlt(g_hbc, px, py, dw, dh, g_hdw, 0, 0, g_ww, g_wh, RGB(255,0,255));
            }

            // Вспышка урона
            if (state->players[p].damage_flash_time > 0.0f) {
                HDC hR = CreateCompatibleDC(g_hbc); 
                HBITMAP hmR = CreateCompatibleBitmap(g_hbc, 10, 10); 
                SelectObject(hR, hmR);
                
                RECT rr = {0, 0, 10, 10}; 
                HBRUSH rb = CreateSolidBrush(RGB(255, 0, 0)); 
                FillRect(hR, &rr, rb); 
                DeleteObject(rb);
                
                BLENDFUNCTION bf = { AC_SRC_OVER, 0, (BYTE)(150 * (state->players[p].damage_flash_time / 0.2f)), 0 };
                AlphaBlend(g_hbc, 0, start_y, 800, view_h, hR, 0, 0, 10, 10, bf); 
                
                DeleteObject(hmR); 
                DeleteDC(hR);
            }
            
            SelectClipRgn(g_hbc, NULL); 
            DeleteObject(clipRgn);
        }

        // ОТРИСОВКА ПАНЕЛИ HUD
        int hud_y = 500;
        RECT hR = {0, hud_y, 800, 600}; 
        HBRUSH hB = CreateSolidBrush(RGB(40, 40, 40)); 
        FillRect(g_hbc, &hR, hB); 
        DeleteObject(hB);
        
        HPEN border = CreatePen(PS_SOLID, 4, RGB(120, 120, 120)); 
        HPEN oldPen = SelectObject(g_hbc, border);
        MoveToEx(g_hbc, 0, hud_y, NULL); 
        LineTo(g_hbc, 800, hud_y); 
        
        if (state->mode == MODE_GAME_COOP) { 
            MoveToEx(g_hbc, 400, hud_y, NULL); 
            LineTo(g_hbc, 400, 600); 
        }
        
        SelectObject(g_hbc, oldPen); 
        DeleteObject(border);

        SetBkMode(g_hbc, TRANSPARENT); 
        HFONT hf = CreateFontA(36, 0, 0, 0, FW_BOLD, 0, 0, 0, 0, 0, 0, 0, 0, "Impact"); 
        SelectObject(g_hbc, hf);
        
        for (int p = 0; p < num_views; p++) {
            int cx = (state->mode == MODE_GAME_SOLO) ? 200 : (p == 0 ? 0 : 400); 
            int face_idx = 3; 
            
            if (state->players[p].hp > 74) face_idx = 0; 
            else if (state->players[p].hp > 49) face_idx = 1; 
            else if (state->players[p].hp > 24) face_idx = 2;
            
            if (!state->players[p].alive) face_idx = 3;

            if (g_hbmFaces[face_idx]) {
                StretchBlt(g_hbc, cx + 20, hud_y + 10, 80, 80, g_hdcFaces[face_idx], 0, 0, g_faceWidth[face_idx], g_faceHeight[face_idx], SRCCOPY);
            }
            
            char s[128];
            if (state->has_won) { 
                snprintf(s, 128, "VICTORY!"); 
                SetTextColor(g_hbc, RGB(50, 255, 50)); 
                TextOutA(g_hbc, cx + 120, hud_y + 30, s, strlen(s)); 
            } else if (state->players[p].alive) { 
                snprintf(s, 128, "HP: %d  AM: %d", state->players[p].hp, state->players[p].ammo); 
                SetTextColor(g_hbc, state->players[p].hp <= 24 ? RGB(255, 0, 0) : RGB(255, 255, 0)); 
                TextOutA(g_hbc, cx + 120, hud_y + 30, s, strlen(s)); 
            } else { 
                snprintf(s, 128, "DEAD (PRESS 'R')"); 
                SetTextColor(g_hbc, RGB(255, 0, 0)); 
                TextOutA(g_hbc, cx + 120, hud_y + 30, s, strlen(s)); 
            }
        }
        DeleteObject(hf);
    }

    HDC hdc = GetDC(g_hwnd);
    if (g_fullscreen) {
        StretchBlt(hdc, 0, 0, g_screenWidth, g_screenHeight, g_hbc, 0, 0, 800, 600, SRCCOPY);
    } else {
        BitBlt(hdc, 0, 0, 800, 600, g_hbc, 0, 0, SRCCOPY);
    }
    ReleaseDC(g_hwnd, hdc);
}

void UI_Shutdown(void) {
    for (int i = 1; i <= 5; i++) {
        if (g_hbt[i]) { 
            SelectObject(g_hdt[i], g_ot[i]); 
            DeleteObject(g_hbt[i]); 
            DeleteDC(g_hdt[i]); 
        }
    }
    for (int i = 0; i < 7; i++) {
        if (g_hbs[i]) { 
            SelectObject(g_hds[i], g_os[i]); 
            DeleteObject(g_hbs[i]); 
            DeleteDC(g_hds[i]); 
        }
    }
    for (int i = 0; i < 4; i++) {
        if (g_hbmFaces[i]) { 
            SelectObject(g_hdcFaces[i], g_hbmOldFaces[i]); 
            DeleteObject(g_hbmFaces[i]); 
            DeleteDC(g_hdcFaces[i]); 
        }
    }
    if (g_hbw) { 
        SelectObject(g_hdw, g_ow); 
        DeleteObject(g_hbw); 
        DeleteDC(g_hdw); 
    }
    
    SelectObject(g_hbc, g_old); 
    DeleteObject(g_hbm); 
    DeleteDC(g_hbc); 
    DestroyWindow(g_hwnd);
    ShowCursor(TRUE);
}