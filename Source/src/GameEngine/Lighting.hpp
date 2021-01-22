#pragma once

#include <Core/Engine.h>

namespace GameEngine {
	enum class LightType :int { Directional, Point, Spot };

	struct Light {
		LightType type;

		glm::vec3 position;
		glm::vec3 direction;

		glm::vec3 ambient;
		glm::vec3 diffuse;
		glm::vec3 specular;

		// The light atenuation (point lights, ..)
		float constant;
		float linear;
		float quadratic;
		
		// Spot light cutoff (smootf/soft edged)
		float cutOff;
		float outerCutOff;
	};

	struct Material {
		glm::vec3 color;
		glm::vec3 diffuse;
		glm::vec3 specular;
		float shininess;
	};
}
