	#include "Renderer.h"

	#include "Walnut/Random.h"
namespace Utils
{
	static uint32_t ConvertToRGBA(glm::vec4& color)
	{
		uint32_t r = (uint32_t)(color.r * 255.0f);
		uint32_t g = (uint32_t)(color.g * 255.0f);
		uint32_t b = (uint32_t)(color.b * 255.0f);
		uint32_t a = (uint32_t)(color.a * 255.0f);

		uint32_t result = (a << 24) | (b << 16) | (g << 8 )| r;

		return result;
	}
}
	void Renderer::OnResize(uint32_t width, uint32_t height)
	{
		if (m_FinalImage)
		{	
			//No resize necessary
			if (m_FinalImage->GetWidth() == width && m_FinalImage->GetHeight() == height)
				return;

			m_FinalImage->Resize(width, height);

		}
		else
		{
			m_FinalImage = std::make_shared < Walnut::Image>(width, height, Walnut::ImageFormat::RGBA);
		}
	
		delete[] m_ImageData;
		m_ImageData = new uint32_t[width * height];

	}

	void Renderer::Render()
	{
		float aspectRatio = (float)m_FinalImage->GetHeight() / (float)m_FinalImage->GetHeight();

		for (uint32_t y = 0; y <  m_FinalImage->GetHeight(); y++)
		{
			for (uint32_t x = 0; x < m_FinalImage->GetWidth(); x++)
			{ 
				
				glm::vec2 coord = { (float)x / (float)m_FinalImage->GetWidth(), (float)y  / (float)m_FinalImage->GetHeight() };
				coord = coord * 2.0f - 1.0f; // -1 -> 1, not 0->1
				coord.x *= aspectRatio;

				glm::vec4 color = PerPixel(coord);
				color = glm::clamp(color, glm::vec4(0.0f), glm::vec4(1.0f));

				m_ImageData[x + y * m_FinalImage->GetWidth()] = Utils::ConvertToRGBA(color);

			}
		}



		m_FinalImage->SetData(m_ImageData);
	}

	glm::vec4 Renderer::PerPixel(glm::vec2 coord)
	{
		uint32_t r = uint32_t(coord.x * 255.0f);
		uint32_t g = uint32_t(coord.y * 255.0f);
		
		glm::vec3 rayOrigin(0.0f, 0.0f, 1.0f);
		glm::vec3 rayDirection(coord.x, coord.y, -1.0f);
		float radius = 0.5f;
		//rayDirection = glm::normalize(rayDirection);
		
		//(bx^2 + by^2 )t^2 + (2(axbx + ayby))t + (ax^2 + ay^2 + r^2) = 0
		//where
		// a = ray origin
		// b = ray direction
		//r = radius
		//t = hit distance
		float a = glm::dot(rayDirection, rayDirection);
		float b = 2.0f * glm::dot(rayOrigin, rayDirection);
		float c = glm::dot(rayOrigin, rayOrigin) - radius * radius;

		//Quadratic fromula discriminant:
		float discriminant = b * b - 4.0f * a * c;

		if (discriminant < 0.0f)
			return glm::vec4(0, 0, 0, 1);;


		
		//-b -+ sqrt(discriminant)/(2*a)
		float t0 = (-b + glm::sqrt(discriminant)) / (2.0f * a);
		float closerT = (-b - glm::sqrt(discriminant)) / (2.0f * a);

		glm::vec3 hitPostion0 = rayOrigin + rayDirection * t0;
		glm::vec3 hitPoint = rayOrigin + rayDirection * closerT;
		glm::vec3 normal = glm::normalize(hitPoint);

		glm::vec3 lightDir = glm::normalize(glm::vec3(-1, -1, -1));

		float d = glm::max(glm::dot(normal, -lightDir), 0.0f); //==cos(angle)

		glm::vec3 sphereColor(1, 0, 0);
		sphereColor *= d;

		return glm::vec4(sphereColor, 1.0f);


		
	}

