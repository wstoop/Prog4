#pragma once
#include "Component.h"

namespace dae
{
	class RenderComponent : public Component
	{
	public:
		virtual ~RenderComponent() override = default;

		virtual void Render() const override = 0;
	};
}