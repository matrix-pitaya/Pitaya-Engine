#pragma once

#include<Core/Color/Color.h>
#include<Core/Math/Rect.h>
#include<GPU/Common/Identifier.h>

namespace Pitaya::Render
{
	struct RenderTargetSnapshot
	{
		Pitaya::GPU::Identifier<Pitaya::GPU::FrameBuffer> SceneFrameBuffer = 0;
		Pitaya::GPU::Identifier<Pitaya::GPU::FrameBuffer> SceneInternalFrameBuffer = 0;
		Pitaya::GPU::Identifier<Pitaya::GPU::Texture2D> SceneColorAttachment = 0;	//多采样点下为InternalFBO的颜色缓冲区

		Pitaya::GPU::Identifier<Pitaya::GPU::FrameBuffer> PingPongFrameBuffers[2] = { 0, 0 };
		Pitaya::GPU::Identifier<Pitaya::GPU::Texture2D> PingPongColorAttachments[2] = { 0, 0 };

		Pitaya::GPU::Identifier<Pitaya::GPU::FrameBuffer> FinalFrameBuffer = 0;
		Pitaya::GPU::Identifier<Pitaya::GPU::Texture2D> FinalColorAttachment = 0;

		Pitaya::Core::Color ClearColor = Pitaya::Core::Color::SkyBlue;
		Pitaya::Core::Rect Rect;
		bool ClearDepth = true;
		bool ClearStencil = true;

		bool Multisample = false;
	};
}