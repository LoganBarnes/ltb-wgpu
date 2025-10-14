// ///////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Logan Barnes - All Rights Reserved
// ///////////////////////////////////////////////////////////////////////////////////////
#include "ltb/wgpu/app.hpp"

namespace ltb::wgpu
{

App::App( Callback app_callback )
    : app_callback_( std::move( app_callback ) )
{
}

auto App::run( ) -> void
{
    constexpr auto descriptor = WGPUInstanceDescriptor{ };

    if ( auto* instance = ::wgpuCreateInstance( &descriptor ) )
    {
        spdlog::info( "WGPU instance: {}", fmt::ptr( instance ) );
        instance_ = std::shared_ptr< WGPUInstanceImpl >( instance, ::wgpuInstanceRelease );
    }
    else
    {
        spdlog::error( "Could not initialize WebGPU!" );
        return;
    }

    constexpr auto options = WGPURequestAdapterOptions{ };

    ::wgpuInstanceRequestAdapter(
        instance_.get( ),
        &options,
        WGPURequestAdapterCallbackInfo{
            .nextInChain = nullptr,
            .mode        = WGPUCallbackMode_AllowProcessEvents,
            .callback    = &App::handle_adapter,
            .userdata1   = this,
            .userdata2   = nullptr,
        }
    );
}

auto App::process( ) -> void
{
    if ( instance_ )
    {
        ::wgpuInstanceProcessEvents( instance_.get( ) );
    }
}

auto App::handle_adapter(
    WGPURequestAdapterStatus const status,
    WGPUAdapter const              adapter,
    WGPUStringView const           message,
    void* const                    userdata1,
    void* const                    userdata2
) -> void
{
    utils::ignore( userdata2 );

    if ( WGPURequestAdapterStatus_Success != status )
    {
        spdlog::error( "Could not get WebGPU adapter: {}", message.data );
        return;
    }

    // Manipulate user data
    App* app = static_cast< App* >( userdata1 );

    spdlog::info( "WebGPU adapter: {}", fmt::ptr( adapter ) );
    app->adapter_ = std::shared_ptr< WGPUAdapterImpl >( adapter, ::wgpuAdapterRelease );

    if ( app->app_callback_ )
    {
        app->app_callback_( *app );
    }
}

} // namespace ltb::wgpu
