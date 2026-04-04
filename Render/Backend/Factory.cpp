#include<Render/Backend/OpenGL/OpenGLRenderer.h>

Pitaya::Render::Renderer* Pitaya::Render::Renderer::Factory::Create(Pitaya::Render::API api)
{
	switch (api)
	{
		case Pitaya::Render::API::OpenGL:
			return PITAYA_NEW(Pitaya::Render::OpenGLRenderer);

		case Pitaya::Render::API::Unknown:
			throw std::exception("Unknown Render Backend API");

		default:
			throw std::exception("Unknown Render Backend API");
	}
}