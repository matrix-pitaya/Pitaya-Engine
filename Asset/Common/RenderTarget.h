#pragma once

#include<Core/Color/Color.h>
#include<Core/Identifier/GUID.h>
#include<Core/Container/SlotMap.h>
#include<GPU/Frontend/Buffer/FrameBuffer.h>
#include<Serialize/Serializable.h>

namespace Pitaya::Asset
{
	struct RenderTarget : Pitaya::Serialize::Serializable
	{
        Pitaya::Core::SlotMap<Pitaya::GPU::FrameBuffer>::Handle SceneFrameBufferHandle;
		Pitaya::GPU::FrameBufferSpecification SceneFrameBufferSpecification = { 1600, 900, 4, Pitaya::GPU::PixelFormat::RGBA16F, Pitaya::GPU::PixelFormat::Depth24_Stencil8 };

		Pitaya::Core::SlotMap<Pitaya::GPU::FrameBuffer>::Handle PingPongFrameBufferHandles[2] = { };
		Pitaya::GPU::FrameBufferSpecification PingPongFrameBufferSpecification = { 1600, 900, 1, Pitaya::GPU::PixelFormat::RGBA16F, Pitaya::GPU::PixelFormat::Invalid };

		Pitaya::Core::SlotMap<Pitaya::GPU::FrameBuffer>::Handle FinalFrameBufferHandle;
		Pitaya::GPU::FrameBufferSpecification FinalFrameBufferSpecification = { 1600, 900, 1, Pitaya::GPU::PixelFormat::RGBA8,    Pitaya::GPU::PixelFormat::Invalid };

		Pitaya::Core::Color ClearColor = Pitaya::Core::Color::SkyBlue;
		bool ClearDepth = true;
		bool ClearStencil = true;

    private:
        void Serialize(Pitaya::Serialize::SerializeContext& context) const override
        {
            auto SerializeFrameBufferSpecification = [](Pitaya::Serialize::SerializeContext& context, const std::string& name, const Pitaya::GPU::FrameBufferSpecification& spec)
                {
                    auto& subContext = context.GetSubContext(name);
                    subContext.Write("Width", spec.Width);
                    subContext.Write("Height", spec.Height);
                    subContext.Write("Samples", spec.Samples);
                    subContext.Write("ColorFormat", std::string(Pitaya::GPU::ToString(spec.ColorFormat)));
                    subContext.Write("DepthFormat", std::string(Pitaya::GPU::ToString(spec.DepthFormat)));
                };
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
            auto DeserializeFrameBufferSpecification = [](const Pitaya::Serialize::DeserializeContext& context, const std::string& name, Pitaya::GPU::FrameBufferSpecification& spec)
                {
                    if (context.HasSubContext(name))
                    {
                        const auto& subContext = context.GetSubContext(name);
                        int out_int; bool out_bool; std::string out_fmt;
                        if (subContext.Read("Width", out_int)) { spec.Width = out_int; }
                        if (subContext.Read("Height", out_int)) { spec.Height = out_int; }
                        if (subContext.Read("Samples", out_int)) { spec.Samples = out_int; }
                        if (subContext.Read("ColorFormat", out_fmt)) { spec.ColorFormat = Pitaya::GPU::ToEnum<Pitaya::GPU::PixelFormat>(out_fmt); }
                        if (subContext.Read("DepthFormat", out_fmt)) { spec.DepthFormat = Pitaya::GPU::ToEnum<Pitaya::GPU::PixelFormat>(out_fmt); }
                    }
                };
            DeserializeFrameBufferSpecification(context, "SceneSpec", SceneFrameBufferSpecification);
            DeserializeFrameBufferSpecification(context, "PingPongSpec", PingPongFrameBufferSpecification);
            DeserializeFrameBufferSpecification(context, "FinalSpec", FinalFrameBufferSpecification);

            if (context.HasSubContext("Clear"))
            {
                const auto& clear = context.GetSubContext("Clear");
                glm::vec4 out_vec4; bool out_bool;
                if (clear.Read("Color", out_vec4)) { ClearColor = out_vec4; }
                if (clear.Read("Depth", out_bool)) { ClearDepth = out_bool; }
                if (clear.Read("Stencil", out_bool)) { ClearStencil = out_bool; }
            }
        }
    };
}
