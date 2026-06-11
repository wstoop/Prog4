#pragma once
#include "EventManager.h"

namespace dae { class GameObject; }

struct ScoreEvent { dae::GameObject* killer; int points; };
constexpr EventId DATEVENT_SCORE_CHANGED = make_sdbm_hash("DATEVENT_SCORE_CHANGED");

struct OverlapEvent { dae::GameObject* self; dae::GameObject* other; };
constexpr EventId DATEVENT_ACTOR_OVERLAPPED = make_sdbm_hash("DATEVENT_ACTOR_OVERLAPPED");

struct LostLifeEvent { dae::GameObject* target; };
constexpr EventId DATEVENT_PLAYER_LOST_LIFE = make_sdbm_hash("DATEVENT_PLAYER_LOST_LIFE");

constexpr EventId EVENT_GAME_OVER = make_sdbm_hash("EVENT_GAME_OVER");
constexpr EventId EVENT_LOAD_START = make_sdbm_hash("EVENT_LOAD_START");
constexpr EventId EVENT_LOAD_SINGLE = make_sdbm_hash("EVENT_LOAD_SINGLE");
constexpr EventId EVENT_LOAD_COOP = make_sdbm_hash("EVENT_LOAD_COOP");
constexpr EventId EVENT_LOAD_VERSUS = make_sdbm_hash("EVENT_LOAD_VERSUS");
constexpr EventId EVENT_LOAD_END = make_sdbm_hash("EVENT_LOAD_END");
constexpr EventId EVENT_LOAD_HIGHSCORE = make_sdbm_hash("EVENT_LOAD_HIGHSCORE");
constexpr EventId EVENT_WAVE_CLEARED = make_sdbm_hash("EVENT_WAVE_CLEARED");
constexpr EventId EVENT_ENEMY_DIED = make_sdbm_hash("EVENT_ENEMY_DIED");
constexpr EventId EVENT_PLAYER_SHOT = make_sdbm_hash("EVENT_PLAYER_SHOT");
constexpr EventId EVENT_PLAYER_DAMAGED_ENEMY = make_sdbm_hash("EVENT_PLAYER_DAMAGED_ENEMY");
constexpr EventId EVENT_FIGHTER_CAPTURED = make_sdbm_hash("EVENT_FIGHTER_CAPTURED");
constexpr EventId EVENT_ALL_ENEMIES_RETURNED = make_sdbm_hash("EVENT_ALL_ENEMIES_RETURNED");
constexpr EventId EVENT_PLAYER_TOOK_DAMAGE = make_sdbm_hash("EVENT_PLAYER_TOOK_DAMAGE");
