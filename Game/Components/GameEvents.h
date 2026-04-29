#pragma once
#include "EventManager.h"

namespace dae { class GameObject; }

struct ScoreEvent { dae::GameObject* killer; int points; };
constexpr EventId SCORE_CHANGED = make_sdbm_hash("ScoreChanged");

struct OverlapEvent { dae::GameObject* self; dae::GameObject* other; };
constexpr EventId ACTOR_OVERLAPPED = make_sdbm_hash("ActorOverlapped");

struct LostLifeEvent { dae::GameObject* target; };
constexpr EventId PLAYER_LOST_LIFE = make_sdbm_hash("PlayerLostLife");

constexpr EventId GAME_OVER = make_sdbm_hash("GameOver");