#pragma once

#include<cstdint>

namespace Pitaya::Core
{
	enum class AssetState : uint32_t
	{
		Empty = 0,				//空标志位

		CPULoading = 1 << 0,	//CPU加载中
		CPULoaded = 1 << 1,		//CPU加载完成
		CPUFailed = 1 << 2,		//CPU加载失败

		GPULoading = 1 << 3,	//GPU加载中
		GPULoaded = 1 << 4,		//GPU加载完成
		GPUFailed = 1 << 5,		//GPU加载失败

		Unload = 1 << 6			//标记为卸载
	};
}
