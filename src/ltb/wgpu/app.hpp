// ///////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Logan Barnes - All Rights Reserved
// ///////////////////////////////////////////////////////////////////////////////////////
#pragma once

// project
#include "ltb/utils/result.hpp"

// external
#include <spdlog/spdlog.h>
#include <webgpu/webgpu.h>

namespace ltb::wgpu
{

class App
{
public:
    using Callback = std::function< void( App& ) >;

    explicit App( Callback app_callback );

    auto run( ) -> void;

    auto process( ) -> void;

private:
    Callback app_callback_;

    std::shared_ptr< WGPUInstanceImpl > instance_ = nullptr;
    std::shared_ptr< WGPUAdapterImpl >  adapter_  = nullptr;
    std::shared_ptr< WGPUDeviceImpl >   device_   = nullptr;
    std::shared_ptr< WGPUQueueImpl >    queue_    = nullptr;

    static auto handle_adapter(
        WGPURequestAdapterStatus status,
        WGPUAdapterImpl*         adapter,
        WGPUStringView           message,
        void*                    userdata1,
        void*                    userdata2
    ) -> void;

    static auto handle_device(
        WGPURequestDeviceStatus status,
        WGPUDeviceImpl*         device,
        WGPUStringView          message,
        void*                   userdata1,
        void*                   userdata2
    ) -> void;
};

} // namespace ltb::wgpu
