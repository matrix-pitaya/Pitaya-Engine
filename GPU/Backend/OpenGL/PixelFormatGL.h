#pragma once

#include<GPU/Common/PixelFormat.h>

#if defined(PITAYA_GRAPHICS_OPENGL)
#include<GL/glew.h>

namespace Pitaya::GPU
{
	struct GLFormatTriplet
	{
		GLenum InternalFormat;
		GLenum Format;
		GLenum Type;
	};

	inline GLFormatTriplet PixelFormatToGL(PixelFormat format) noexcept
	{
		switch (format)
		{
			case PixelFormat::R8:				return { GL_R8,					GL_RED,				GL_UNSIGNED_BYTE };
			case PixelFormat::RG8:				return { GL_RG8,				GL_RG,				GL_UNSIGNED_BYTE };
			case PixelFormat::RGB8:				return { GL_RGB8,				GL_RGB,				GL_UNSIGNED_BYTE };
			case PixelFormat::RGBA8:			return { GL_RGBA8,				GL_RGBA,			GL_UNSIGNED_BYTE };
			case PixelFormat::SRGB8_A8:			return { GL_SRGB8_ALPHA8,		GL_RGBA,			GL_UNSIGNED_BYTE };
			case PixelFormat::R16F:				return { GL_R16F,				GL_RED,				GL_FLOAT };
			case PixelFormat::RG16F:			return { GL_RG16F,				GL_RG,				GL_FLOAT };
			case PixelFormat::RGB16F:			return { GL_RGB16F,				GL_RGB,				GL_FLOAT };
			case PixelFormat::RGBA16F:			return { GL_RGBA16F,			GL_RGBA,			GL_FLOAT };
			case PixelFormat::R32F:				return { GL_R32F,				GL_RED,				GL_FLOAT };
			case PixelFormat::Depth24_Stencil8:	return { GL_DEPTH24_STENCIL8,	GL_DEPTH_STENCIL,	GL_UNSIGNED_INT_24_8 };
			case PixelFormat::Depth32F:			return { GL_DEPTH_COMPONENT32F,	GL_DEPTH_COMPONENT,	GL_FLOAT };
			case PixelFormat::Invalid:			return { 0,						0,					0 };
			default:							return { 0,						0,					0 };
		}
	}
	inline GLenum DepthFormatToAttachment(PixelFormat format) noexcept
	{
		switch (format)
		{
			case PixelFormat::Depth24_Stencil8:	return GL_DEPTH_STENCIL_ATTACHMENT;
			case PixelFormat::Depth32F:			return GL_DEPTH_ATTACHMENT;
			default:							return GL_NONE;
		}
	}
}
#endif
