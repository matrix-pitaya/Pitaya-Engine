#pragma once

#include<Core/Color/Color.h>
#include<Core/Math/Rect.h>
#include<Core/Camera/CameraSnapshot.h>
#include<Render/Common/RenderCommandType.h>
#include<Render/Common/PostProcessSetting.h>

#include<glm.hpp>

namespace Pitaya::Render
{
	struct BeginPassCommand
	{
		inline static constexpr const RenderCommandType Type = RenderCommandType::BeginPass;

		Pitaya::Core::CameraSnapshot CameraSnapshot;

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
