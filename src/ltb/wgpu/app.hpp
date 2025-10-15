// ///////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Logan Barnes - All Rights Reserved
// ///////////////////////////////////////////////////////////////////////////////////////
#pragma once

// project
#include "ltb/utils/result.hpp"
#include "ltb/window/os_window.hpp"

// external
#include <spdlog/spdlog.h>
#include <webgpu/webgpu.h>

namespace ltb::wgpu
{

class App;

using AppCallback = std::function< void( App& ) >;

struct AppSettings
{
    AppCallback       callback = nullptr;
    window::OsWindow* window   = nullptr;
};

class App
{
public:
    explicit App( );
    explicit App( AppSettings app_settings );

    auto run( ) -> void;

    auto process( ) -> void;

    static constexpr glm::uvec2 default_size = { 1280U, 720U };

private:
    AppCallback app_callback_;

    window::OsWindow*                   window_   = nullptr;
    std::shared_ptr< WGPUInstanceImpl > instance_ = nullptr;
    std::shared_ptr< WGPUSurfaceImpl >  surface_  = nullptr;

    std::shared_ptr< WGPUAdapterImpl > adapter_ = nullptr;
    std::shared_ptr< WGPUDeviceImpl >  device_  = nullptr;
    std::shared_ptr< WGPUQueueImpl >   queue_   = nullptr;

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

    static auto configure_surface( ) -> void;
};

} // namespace ltb::wgpu
