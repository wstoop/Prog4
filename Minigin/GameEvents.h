#pragma once
#include "EventManager.h"

namespace dae { class GameObject; }

struct ScoreEvent { dae::GameObject* killer; int points; };
constexpr EventId SCORE_CHANGED = make_sdbm_hash("ScoreChanged");

struct OverlapData { dae::GameObject* self; dae::GameObject* other; };
constexpr EventId ACTOR_OVERLAPPED = make_sdbm_hash("ActorOverlapped");