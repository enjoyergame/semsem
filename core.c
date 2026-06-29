#include "core.h"

static bool CanMove(GameState* state, float x, float y) {
    int ix = (int)x;
    int iy = (int)y;
    if(ix < 0 || ix >= MAP_WIDTH || iy < 0 || iy >= MAP_HEIGHT) return false;
    return state->map[iy][ix] == 0;
}

void StartGame(GameState* state, GameMode mode) {
    state->mode = mode;
    state->has_won = false;
    state->play_damage_sound = false;
    
    // Карта 32x32
    int initial_map[MAP_HEIGHT][MAP_WIDTH] = {
        {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
        {1,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1},
        {1,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,2,0,0,0,0,0,0,0,0,0,0,0,0,2,1},
        {1,0,0,0,0,0,1,0,1,1,1,1,4,1,1,0,1,2,0,2,2,2,0,0,2,2,2,0,0,0,2,1},
        {1,0,0,0,0,0,4,0,1,0,0,0,0,0,1,0,4,4,0,2,2,2,0,0,2,2,2,0,0,0,2,1},
        {1,0,0,0,0,0,1,0,1,0,1,1,1,0,1,0,1,2,0,0,0,0,0,0,0,0,0,0,0,0,2,1},
        {1,1,1,4,1,1,1,0,1,0,1,0,1,0,1,0,1,2,2,2,2,2,4,4,2,2,2,2,2,2,2,1},
        {1,0,0,0,0,0,0,0,1,0,1,0,1,0,1,0,1,1,1,1,1,1,0,0,1,1,1,1,1,1,1,1},
        {1,0,0,0,0,0,0,0,1,0,1,0,0,0,1,0,0,0,0,0,0,1,0,0,1,0,0,0,0,0,0,1},
        {1,0,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,0,1,0,0,1,0,3,3,3,3,0,1},
        {1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1,0,0,1,0,3,0,0,3,0,1},
        {1,0,1,0,3,3,3,3,3,3,3,0,0,0,0,0,0,0,0,1,0,1,1,1,1,0,3,0,0,3,0,1},
        {1,0,4,0,3,0,0,0,0,0,3,0,0,1,1,1,1,0,0,1,0,0,0,0,0,0,3,0,0,3,0,1},
        {1,0,1,0,3,0,0,0,0,0,3,0,0,1,0,0,1,0,0,1,1,1,1,1,1,1,3,3,4,3,0,1},
        {1,0,1,0,3,0,0,0,0,0,3,0,0,1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,1,0,3,3,3,4,3,3,3,0,0,1,0,0,1,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1},
        {1,0,1,0,0,0,0,0,0,0,0,0,0,1,0,0,1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,1,1,1,1,1,1,1,1,1,1,1,1,0,0,1,0,0,1,0,2,2,2,2,2,2,2,2,2,0,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1,0,2,0,0,0,0,0,0,0,2,0,1},
        {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,4,0,2,0,0,0,0,0,0,0,2,0,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,2,0,0,0,0,0,0,0,2,0,1},
        {1,0,2,2,2,2,2,0,2,2,2,2,2,2,2,2,2,2,2,1,0,2,2,2,2,4,4,2,2,2,0,1},
        {1,0,2,0,0,0,2,0,2,0,0,0,0,0,0,0,0,0,2,1,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,2,0,0,0,2,0,2,0,0,0,0,0,0,0,0,0,2,1,0,1,1,1,1,1,1,1,1,1,1,1},
        {1,0,2,0,0,0,2,4,2,0,0,3,3,3,3,0,0,0,2,1,0,1,0,0,0,0,0,0,0,0,0,1},
        {1,0,2,2,4,2,2,0,2,0,0,3,0,0,3,0,0,0,2,1,0,1,0,0,0,0,0,0,0,0,0,1},
        {1,0,0,0,0,0,0,0,2,0,0,3,0,0,3,0,0,0,2,1,0,1,0,0,0,0,0,0,0,0,0,1},
        {1,0,2,2,2,2,2,2,2,0,0,3,3,3,3,0,0,0,2,1,0,1,0,0,0,0,0,0,0,0,0,1},
        {1,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,1,0,1,1,1,1,4,1,1,1,1,0,1},
        {1,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,1,0,0,0,0,0,0,0,0,0,5,0,1},
        {1,0,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1,1,1,1,1,1,1,1,1,1,1,1,1},
        {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
    };

    for(int y = 0; y < MAP_HEIGHT; ++y)
        for(int x = 0; x < MAP_WIDTH; ++x)
            state->map[y][x] = initial_map[y][x];
    
    // Инициализация игроков
    state->players[0] = (Player){2.5f, 2.5f, 1.0f, 0.0f, 0.0f, 0.66f, 100, 20, 0.0f, 0.0f, 0.0f, true};
    state->players[1] = (Player){2.5f, 3.5f, 1.0f, 0.0f, 0.0f, 0.66f, 100, 20, 0.0f, 0.0f, 0.0f, true};
    if(mode != MODE_GAME_COOP) state->players[1].alive = false;
    
    // Расстановка врагов
    float enemiesX[15] = {2.5f, 7.5f, 11.5f, 18.5f, 28.5f, 22.5f, 28.5f, 8.5f, 21.5f, 5.5f, 14.5f, 5.5f, 13.5f, 25.5f, 26.5f};
    float enemiesY[15] = {7.5f, 2.5f, 4.5f, 3.5f, 2.5f, 6.5f, 8.5f, 10.5f, 12.5f, 14.5f, 15.5f, 23.5f, 23.5f, 18.5f, 28.5f};
    int enemiesType[15] = {0, 0, 1, 1, 0, 1, 0, 0, 1, 0, 1, 0, 1, 1, 0};

    for(int i = 0; i < MAX_ENEMIES; ++i) state->enemies[i] = (Enemy){enemiesX[i], enemiesY[i], enemiesType[i], 100, 0, true, 0, 0, 0};
    
    for(int i = 0; i < MAX_PROJECTILES; ++i) state->projectiles[i].active = false;
    
    // Расстановка предметов
    float itemsX[13] = {2.5f, 9.5f, 22.5f, 28.5f, 18.5f, 14.5f, 28.5f, 8.5f, 5.5f, 12.5f, 28.5f, 22.5f, 28.5f};
    float itemsY[13] = {12.5f, 8.5f, 2.5f, 5.5f, 10.5f, 13.5f, 12.5f, 16.5f, 28.5f, 25.5f, 18.5f, 28.5f, 28.5f};
    int itemsType[13] = {1, 0, 1, 0, 1, 1, 0, 1, 0, 1, 1, 0, 1};

    for(int i = 0; i < MAX_ITEMS; ++i) state->items[i] = (Item){itemsX[i], itemsY[i], itemsType[i], true};
}

void Core_Init(GameState* state) {
    state->mode = MODE_MENU;
}

void Core_Update(GameState* state, const InputState* input, float delta_time) {
    if(state->mode == MODE_MENU) {
        if(input->k1) StartGame(state, MODE_GAME_SOLO);
        if(input->k2) StartGame(state, MODE_GAME_COOP);
        return;
    }
    if(input->reset) {
        Core_Init(state);
        return;
    }
    if(state->has_won) return;

    float move_speed = 4.0f * delta_time;
    float rot_speed = 3.0f * delta_time;

    // ОБРАБОТКА ЛОГИКИ ИГРОКОВ
    for(int p = 0; p < 2; ++p) {
        Player* pl = &state->players[p];
        if(!pl->alive) continue;

        if(pl->weapon_cooldown > 0) pl->weapon_cooldown -= delta_time;
        if(pl->weapon_anim_time > 0) pl->weapon_anim_time -= delta_time;
        if(pl->damage_flash_time > 0) pl->damage_flash_time -= delta_time;

        bool i_up = (p == 0) ? input->w : input->up;
        bool i_down = (p == 0) ? input->s : input->down;
        bool i_left = (p == 0) ? input->a : input->left;
        bool i_right = (p == 0) ? input->d : input->right;
        bool i_shoot = (p == 0) ? input->space : input->enter;

        // Стрельба
        if(i_shoot && pl->weapon_cooldown <= 0 && pl->ammo > 0) {
            pl->ammo--;
            pl->weapon_cooldown = 0.5f;
            pl->weapon_anim_time = 0.2f;
            float bx = pl->x;
            float by = pl->y;
            for(int i = 0; i < 150; ++i) {
                bx += pl->dir_x * 0.1f;
                by += pl->dir_y * 0.1f;
                if(state->map[(int)by][(int)bx] > 0 && state->map[(int)by][(int)bx] != 5) {
                    if(state->map[(int)by][(int)bx] == 4) state->map[(int)by][(int)bx] = 0;
                    break;
                }
                for(int e = 0; e < MAX_ENEMIES; ++e) {
                    if(state->enemies[e].alive) {
                        float dx = bx - state->enemies[e].x;
                        float dy = by - state->enemies[e].y;
                        if(dx*dx + dy*dy < 0.25f) {
                            state->enemies[e].hp -= 50;
                            if(state->enemies[e].hp <= 0) state->enemies[e].alive = false;
                            i = 150;
                            break;
                        }
                    }
                }
                if(state->mode == MODE_GAME_COOP) {
                    int other_p = 1 - p;
                    if(state->players[other_p].alive) {
                        float dx = bx - state->players[other_p].x;
                        float dy = by - state->players[other_p].y;
                        if(dx*dx + dy*dy < 0.25f) {
                            state->players[other_p].hp -= 20;
                            state->players[other_p].damage_flash_time = 0.2f;
                            state->play_damage_sound = true;
                            if(state->players[other_p].hp <= 0) state->players[other_p].alive = false;
                            break;
                        }
                    }
                }
            }
        }

        // Предметы
        for(int i = 0; i < MAX_ITEMS; ++i) {
            if(state->items[i].active) {
                float dx = pl->x - state->items[i].x;
                float dy = pl->y - state->items[i].y;
                if(dx*dx + dy*dy < 0.3f) {
                    state->items[i].active = false;
                    if(state->items[i].type == 0) pl->hp = __min(pl->hp + 50, 100);
                    else if(state->items[i].type == 1) pl->ammo += 10;
                }
            }
        }

        // Движение игрока
        if(i_up) {
            float px = pl->dir_x > 0 ? 0.2f : -0.2f;
            float py = pl->dir_y > 0 ? 0.2f : -0.2f;
            int tX = state->map[(int)pl->y][(int)(pl->x + pl->dir_x * move_speed + px)];
            int tY = state->map[(int)(pl->y + pl->dir_y * move_speed + py)][(int)pl->x];
            if(tX == 0 || tX == 5) {
                pl->x += pl->dir_x * move_speed;
                if (tX == 5) state->has_won = true;
            }
            if(tY == 0 || tY == 5) {
                pl->y += pl->dir_y * move_speed;
                if (tY == 5) state->has_won = true;
            }
        }
        if(i_down) {
            float px = -pl->dir_x > 0 ? 0.2f : -0.2f;
            float py = -pl->dir_y > 0 ? 0.2f : -0.2f;
            int tX = state->map[(int)pl->y][(int)(pl->x - pl->dir_x * move_speed + px)];
            int tY = state->map[(int)(pl->y - pl->dir_y * move_speed + py)][(int)pl->x];
            if(tX == 0 || tX == 5) {
                pl->x -= pl->dir_x * move_speed;
                if(tX == 5) state->has_won = true;
            }
            if(tY == 0 || tY == 5) {
                pl->y -= pl->dir_y * move_speed;
                if(tY == 5) state->has_won = true;
            }
        }
        if(i_right || i_left) {
            float rot = i_right ? rot_speed : -rot_speed;
            float odx = pl->dir_x;
            pl->dir_x = pl->dir_x * cos(rot) - pl->dir_y * sin(rot);
            pl->dir_y = odx * sin(rot) + pl->dir_y * cos(rot);
            float opx = pl->plane_x;
            pl->plane_x = pl->plane_x * cos(rot) - pl->plane_y * sin(rot);
            pl->plane_y = opx * sin(rot) + pl->plane_y * cos(rot);
        }
    }

    // Фаерболы
    for(int i = 0; i < MAX_PROJECTILES; ++i) {
        if(state->projectiles[i].active) {
            state->projectiles[i].x += state->projectiles[i].vx * delta_time;
            state->projectiles[i].y += state->projectiles[i].vy * delta_time;
            if(state->map[(int)state->projectiles[i].y][(int)state->projectiles[i].x] > 0 &&
                state->map[(int)state->projectiles[i].y][(int)state->projectiles[i].x] != 5) {
                state->projectiles[i].active = false;
            }
            else {
                for(int p = 0; p < 2; ++p) {
                    if(!state->players[p].alive) continue;
                    float dx = state->players[p].x - state->projectiles[i].x;
                    float dy = state->players[p].y - state->projectiles[i].y;
                    if(dx*dx + dy*dy < 0.3f) {
                        state->players[p].hp -= 15;
                        state->players[p].damage_flash_time = 0.2f;
                        state->play_damage_sound = true;
                        if(state->players[p].hp <= 0) state->players[p].alive = false;
                        state->projectiles[i].active = false;
                        break;
                    }
                }
            }
        }
    }

    // ИИ врагов
    for(int e = 0; e < MAX_ENEMIES; ++e) {
        if(!state->enemies[e].alive) continue;
        if(state->enemies[e].cooldown > 0) state->enemies[e].cooldown -= delta_time;
        float ex = state->enemies[e].x;
        float ey = state->enemies[e].y;
        float min_dist = 15.0f;
        int target_p = -1;
        for(int p = 0; p < 2; ++p) {
            if(!state->players[p].alive) continue;
            float dx = state->players[p].x - ex;
            float dy = state->players[p].y - ey;
            float d = sqrt(dx*dx + dy*dy);
            if(d < min_dist) {
                bool sees = true;
                float sx = ex, sy = ey;
                float stx = dx/d * 0.2f;
                float sty = dy/d * 0.2f;
                for(int i = 0; i < (int)(d / 0.2f); ++i) {
                    sx += stx;
                    sy += sty;
                    if(state->map[(int)sy][(int)sx] > 0 &&
                        state->map[(int)sy][(int)sx] != 4 &&
                        state->map[(int)sy][(int)sx] != 5) {
                        sees = false;
                        break;
                    }
                }
                if(sees) {
                    min_dist = d;
                    target_p = p;
                }
            }
        }

        float e_speed = 1.2f * delta_time;
        if(target_p != -1) {
            float dx = (state->players[target_p].x - ex) / min_dist;
            float dy = (state->players[target_p].y - ey) / min_dist;
            float px = dx > 0 ? 0.3f : -0.3f;
            float py = dy > 0 ? 0.3f : -0.3f;

            if(state->enemies[e].type == 0) {
                if(min_dist > 0.8f) {
                    if(CanMove(state, ex + dx*e_speed + px, ey)) state->enemies[e].x += dx*e_speed;
                    if(CanMove(state, ex, ey + dy*e_speed + py)) state->enemies[e].y += dy*e_speed;
                }
                else if (state->enemies[e].cooldown <= 0) {
                    state->players[target_p].hp -= 10;
                    state->players[target_p].damage_flash_time = 0.2f;
                    state->play_damage_sound = true;
                    if(state->players[target_p].hp <= 0) state->players[target_p].alive = false;
                    state->enemies[e].cooldown = 1.0f;
                }
            }
            else {
                if(min_dist > 5.0f) {
                    if(CanMove(state, ex + dx*e_speed + px, ey)) state->enemies[e].x += dx*e_speed;
                    if(CanMove(state, ex, ey + dy*e_speed + py)) state->enemies[e].y += dy*e_speed;
                }
                else if (min_dist < 4.0f) {
                    float rdx = -dx, rdy = -dy;
                    float rpx = rdx > 0 ? 0.3f : -0.3f;
                    float rpy = rdy > 0 ? 0.3f : -0.3f;
                    if(CanMove(state, ex + rdx*e_speed + rpx, ey)) state->enemies[e].x += rdx*e_speed;
                    if(CanMove(state, ex, ey + rdy*e_speed + rpy)) state->enemies[e].y += rdy*e_speed;
                }
                if(state->enemies[e].cooldown <= 0) {
                    for(int pr = 0; pr < MAX_PROJECTILES; pr++) {
                        if(!state->projectiles[pr].active) {
                            state->projectiles[pr].active = true;
                            state->projectiles[pr].x = ex;
                            state->projectiles[pr].y = ey;
                            state->projectiles[pr].vx = dx * 3.5f;
                            state->projectiles[pr].vy = dy * 3.5f;
                            break;
                        }
                    }
                    state->enemies[e].cooldown = 2.0f;
                }
            }
        }
        else {
            if(state->enemies[e].roam_timer <= 0) {
                float a = (rand() % 360) * 3.14159f / 180.0f;
                state->enemies[e].rx = cos(a);
                state->enemies[e].ry = sin(a);
                state->enemies[e].roam_timer = 1.0f + (rand() % 4);
            }
            state->enemies[e].roam_timer -= delta_time;
            float px = state->enemies[e].rx > 0 ? 0.3f : -0.3f;
            float py = state->enemies[e].ry > 0 ? 0.3f : -0.3f;
            bool hit = false;
            
            if(CanMove(state, ex + state->enemies[e].rx*e_speed + px, ey)) state->enemies[e].x += state->enemies[e].rx*e_speed;
            else hit = true;
            if(CanMove(state, ex, ey + state->enemies[e].ry*e_speed + py)) state->enemies[e].y += state->enemies[e].ry*e_speed;
            else hit = true;
            if (hit) state->enemies[e].roam_timer = 0;
        }
    }

    // RAYCASTING И ОТРИСОВКА ВИДОВ
    for(int p = 0; p < (state->mode == MODE_GAME_COOP ? 2 : 1); ++p) {
        Player* pl = &state->players[p];
        RenderView* view = &state->views[p];
        if(!pl->alive) continue;

        for(int x = 0; x < SCREEN_WIDTH; ++x) {
            float camX = 2.0f * x / (float)SCREEN_WIDTH - 1.0f;
            float rdx = pl->dir_x + pl->plane_x * camX;
            float rdy = pl->dir_y + pl->plane_y * camX;
            int mx = (int)pl->x;
            int my = (int)pl->y;
            float ddx = (rdx == 0) ? 1e30f : fabs(1.0f / rdx);
            float ddy = (rdy == 0) ? 1e30f : fabs(1.0f / rdy);
            float sdx, sdy;
            int sx, sy, hit = 0, side = 0;
            if(rdx < 0) {
                sx = -1;
                sdx = (pl->x - mx) * ddx;
            }
            else {
                sx = 1;
                sdx = (mx + 1.0f - pl->x) * ddx;
            }
            if(rdy < 0) {
                sy = -1;
                sdy = (pl->y - my) * ddy;
            }
            else {
                sy = 1;
                sdy = (my + 1.0f - pl->y) * ddy;
            }
            while(hit == 0) {
                if(sdx < sdy) {sdx += ddx; mx += sx; side = 0;}
                else {sdy += ddy; my += sy; side = 1;}
                if(state->map[my][mx] > 0) hit = state->map[my][mx];
            }
            float pwd = (side == 0) ? (sdx - ddx) : (sdy - ddy);
            if(pwd <= 0.0001f) pwd = 0.0001f;
            float wx = (side == 0) ? (pl->y + pwd * rdy) : (pl->x + pwd * rdx);
            wx -= floor(wx);
            if(side == 0 && rdx > 0) wx = 1.0f - wx;
            if(side == 1 && rdy < 0) wx = 1.0f - wx;
            view->render_buffer[x].wall_height = (int)(600.0f / pwd);
            view->render_buffer[x].hit_type = hit;
            view->render_buffer[x].is_dark = (side == 1);
            view->render_buffer[x].texture_x = wx;
            view->z_buffer[x] = pwd;
        }

        view->num_sprites = 0;
        for(int e = 0; e < MAX_ENEMIES; ++e) {
            if(state->enemies[e].alive) {
                float dx = state->enemies[e].x - pl->x;
                float dy = state->enemies[e].y - pl->y;
                int tex_id = state->enemies[e].type == 0 ? 0 : 1;
                view->render_sprites[view->num_sprites++] = (RenderSprite){state->enemies[e].x, state->enemies[e].y, tex_id, dx*dx+dy*dy};
            }
        }
        for(int pr = 0; pr < MAX_PROJECTILES; ++pr) {
            if(state->projectiles[pr].active) {
                float dx = state->projectiles[pr].x - pl->x;
                float dy = state->projectiles[pr].y - pl->y;
                view->render_sprites[view->num_sprites++] = (RenderSprite){state->projectiles[pr].x, state->projectiles[pr].y, 2, dx*dx+dy*dy};
            }
        }
        for(int i = 0; i < MAX_ITEMS; ++i) {
            if(state->items[i].active) {
                float dx = state->items[i].x - pl->x;
                float dy = state->items[i].y - pl->y;
                int tex_id = state->items[i].type == 0 ? 3 : 4;
                view->render_sprites[view->num_sprites++] = (RenderSprite){state->items[i].x, state->items[i].y, tex_id, dx*dx+dy*dy};
            }
        }
        if(state->mode == MODE_GAME_COOP) {
            int other_p = 1 - p;
            if(state->players[other_p].alive) {
                float dx = state->players[other_p].x - pl->x;
                float dy = state->players[other_p].y - pl->y;
                int tex_id = (other_p == 0) ? 5 : 6;
                view->render_sprites[view->num_sprites++] = (RenderSprite){state->players[other_p].x, state->players[other_p].y, tex_id, dx*dx+dy*dy};
            }
        }

        // Сортировка спрайтов
        for(int i = 0; i < view->num_sprites - 1; ++i) {
            for(int j = 0; j < view->num_sprites - 1 - i; ++j) {
                if(view->render_sprites[j].dist < view->render_sprites[j+1].dist) {
                    RenderSprite t = view->render_sprites[j];
                    view->render_sprites[j] = view->render_sprites[j+1];
                    view->render_sprites[j+1] = t;
                }
            }
        }
    }
}