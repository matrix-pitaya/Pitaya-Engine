#pragma once

#include<Core/Allocate/Allocate.h>
#include<Editor/GUI/Command/GUIDrawCommand.h>
#include<Editor/GUI/ImGui/imgui.h>

namespace Pitaya::Editor
{
	struct GUIDrawCommand
	{
	public:
		GUIDrawCommand()
		{
			Create();
		}
		~GUIDrawCommand()
		{
			Release();
		}

	public:
		GUIDrawCommand(const GUIDrawCommand&) = delete;
		GUIDrawCommand& operator=(const GUIDrawCommand&) = delete;
		GUIDrawCommand(GUIDrawCommand&& other) noexcept : data(other.data)
		{
			other.data = nullptr;
		}
		GUIDrawCommand& operator=(GUIDrawCommand&& other) noexcept
		{
			if (this == &other) { return *this; }

			Release();           
			data = other.data;  
			other.data = nullptr;
			return *this;
		}

	public:
		inline ImDrawData* GetDrawData() const noexcept
		{
			return data;
		}

	private:
		inline void Create()
		{
			ImDrawData* src = ImGui::GetDrawData();
			if (!src || src->CmdListsCount == 0 || src->TotalVtxCount == 0) { return; }

			data = Pitaya::Core::New<ImDrawData>();
			*data = *src;

			data->CmdLists.clear();
			data->CmdLists.reserve(src->CmdListsCount);
			for (int i = 0; i < src->CmdListsCount; i++)
			{
				ImDrawList* srcList = src->CmdLists[i];
				ImDrawList* dstList = srcList->CloneOutput();
				data->CmdLists.push_back(dstList);
			}
		}
		inline void Release()
		{
			if (!data) { return; }
			for (int32_t i = 0; i < data->CmdLists.Size; i++)
			{
				IM_DELETE(data->CmdLists[i]);
			}
			Pitaya::Core::Delete(data);
			data = nullptr;
		}

	private:
		ImDrawData* data = nullptr;
	};
}
