#pragma once

#include<Core/Color/Color.h>
#include<Core/Identifier/GUID.h>

#include<GPU/Frontend/Buffer/FrameBuffer.h>
#include<Serialize/Serializable.h>

namespace Pitaya::Asset
{
	struct RenderTarget : Pitaya::Serialize::Serializable
	{
		Pitaya::GPU::Identifier<Pitaya::GPU::FrameBuffer> SceneFrameBuffer = 0;
        Pitaya::GPU::Identifier<Pitaya::GPU::FrameBuffer> SceneInternalFrameBuffer = 0; //用于多采用解析
		Pitaya::GPU::Identifier<Pitaya::GPU::Texture2D> SceneColorAttachment = 0;
		Pitaya::GPU::FrameBufferSpecification SceneFrameBufferSpecification;

		Pitaya::GPU::Identifier<Pitaya::GPU::FrameBuffer> PingPongFrameBuffers[2] = { 0, 0 };
		Pitaya::GPU::Identifier<Pitaya::GPU::Texture2D> PingPongColorAttachments[2] = { 0, 0 };
		Pitaya::GPU::FrameBufferSpecification PingPongFrameBufferSpecification;

		Pitaya::GPU::Identifier<Pitaya::GPU::FrameBuffer> FinalFrameBuffer = 0;
		Pitaya::GPU::Identifier<Pitaya::GPU::Texture2D> FinalColorAttachment = 0;
		Pitaya::GPU::FrameBufferSpecification FinalFrameBufferSpecification;

		Pitaya::Core::Color ClearColor = Pitaya::Core::Color::SkyBlue;
		bool ClearDepth = true;
		bool ClearStencil = true;

		//RenderTarget 23-24
		inline static constexpr const Pitaya::Core::GUID Game = Pitaya::Core::GUID("00000000-0100-0000-0000-000000000000");
		inline static constexpr const Pitaya::Core::GUID Editor = Pitaya::Core::GUID("00000000-0200-0000-0000-000000000000");

	private:
        inline void SerializeFrameBufferSpecification(Pitaya::Serialize::SerializeContext& context, const std::string& name, const Pitaya::GPU::FrameBufferSpecification& spec) const
        {
            auto& subContext = context.GetSubContext(name);
            subContext.Write("Width", spec.Width);
            subContext.Write("Height", spec.Height);
            subContext.Write("Samples", spec.Samples);
            subContext.Write("SwapChainTarget", spec.SwapChainTarget);
            subContext.Write("HDR", spec.HDR);
            subContext.Write("HasDepth", spec.HasDepth);
        }
        inline void DeserializeFrameBufferSpecification(const Pitaya::Serialize::DeserializeContext& context, const std::string& name, Pitaya::GPU::FrameBufferSpecification& spec)
        {
            if (context.HasSubContext(name))
            {
                const auto& subContext = context.GetSubContext(name);
                int out_int; bool out_bool;
                if (subContext.Read("Width", out_int)) { spec.Width = out_int; }
                if (subContext.Read("Height", out_int)) { spec.Height = out_int; }
                if (subContext.Read("Samples", out_int)) { spec.Samples = out_int; }
                if (subContext.Read("SwapChainTarget", out_bool)) { spec.SwapChainTarget = out_bool; }
                if (subContext.Read("HDR", out_bool)) { spec.HDR = out_bool; }
                if (subContext.Read("HasDepth", out_bool)) { spec.HasDepth = out_bool; }
            }
        }

    private:
        void Serialize(Pitaya::Serialize::SerializeContext& context) const override
        {
            SerializeFrameBufferSpecification(context, "SceneSpec", SceneFrameBufferSpecification);
            SerializeFrameBufferSpecification(context, "PingPongSpec", PingPongFrameBufferSpecification);
            SerializeFrameBufferSpecification(context, "FinalSpec", FinalFrameBufferSpecification);

            auto& clear = context.GetSubContext("Clear");
            clear.Write("Color", ClearColor);
            clear.Write("Depth", ClearDepth);
            clear.Write("Stencil", ClearStencil);
        }
        void Deserialize(const Pitaya::Serialize::DeserializeContext& context) override
        {
            DeserializeFrameBufferSpecification(context, "SceneSpec", SceneFrameBufferSpecification);
            DeserializeFrameBufferSpecification(context, "PingPongSpec", PingPongFrameBufferSpecification);
            DeserializeFrameBufferSpecification(context, "FinalSpec", FinalFrameBufferSpecification);

            glm::vec4 out_vec4; bool out_bool;
            if (context.HasSubContext("Clear"))
            {
                const auto& clear = context.GetSubContext("Clear");
                if (clear.Read("Color", out_vec4)) { ClearColor = out_vec4; }
                if (clear.Read("Depth", out_bool)) { ClearDepth = out_bool; }
                if (clear.Read("Stencil", out_bool)) { ClearStencil = out_bool; }
            }
        }
    };
}
