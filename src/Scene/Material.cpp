#include "Material.h"

namespace Rutile {
	Material::Material(glm::vec3 diffuse, glm::vec3 ambient, glm::vec3 specular) 
		: diffuse(diffuse), ambient(ambient), specular(specular) {

	}

	TexturedMaterial::TexturedMaterial(Image diffuse, Image ambient, Image specular) 
		: Material(GetAverageImageColour(diffuse), GetAverageImageColour(ambient), GetAverageImageColour(specular)) {

	}

	glm::vec3 TexturedMaterial::GetAverageImageColour(Image image) {

	}
}