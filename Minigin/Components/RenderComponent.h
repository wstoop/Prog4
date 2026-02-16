#pragma once
#include <memory>
#include "Component.h"

namespace dae
{
	class Texture2D;

	class RenderComponent : public Component
	{
	public:
		virtual ~RenderComponent() override = default;

		void Render() const;
	protected:
		std::shared_ptr<Texture2D> m_Texture;
	};
}