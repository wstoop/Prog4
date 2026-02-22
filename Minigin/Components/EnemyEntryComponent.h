#pragma once
#include "Component.h"
#include <glm/glm.hpp>

namespace dae
{
	class TransformComponent;
	class EnemyEntryComponent final : public Component
	{
		enum class SegmentType { Line, Arc };

		struct Segment
		{
			SegmentType type;
			// Line
			glm::vec3 p0{}, p3{};
			// Arc
			glm::vec3 center{};
			float radius{};
			float startAngle{};
			float sweepAngle{};

			float timeStart{};
			float timeEnd{};
		};
		std::vector<Segment> m_segments;

		glm::vec3 m_endPoint{};
		TransformComponent* m_transform{ nullptr };
		GameObject* m_formationParent{ nullptr };

		float m_duration{ 1.f };
		float m_elapsed{ 0.f };
		float m_startDelay{ 0.f };
		bool m_done{ false };
		bool m_start{false};
		bool m_fromLeft{ true };
		void BasicTopEntry(bool fromLeft);
		void ComputeSegmentTimeRanges();

	public:
		EnemyEntryComponent(GameObject* owner, GameObject* formationParent, TransformComponent* transform, const glm::vec3& target, float duration, float startDelay);
		void Update() override;
		void StartEntryTop(bool fromLeft);
		bool GetFromLeft() const { return m_fromLeft; }
		bool IsDocked() const { return m_done; }
		void SetEntryDirection(bool fromLeft) { m_fromLeft = fromLeft; }
	};
}