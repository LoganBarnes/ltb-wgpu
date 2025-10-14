// ///////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Logan Barnes - All Rights Reserved
// ///////////////////////////////////////////////////////////////////////////////////////
#include "ltb/wgpu/app.hpp"

// project
#include "ltb/wgpu/enum_strings.hpp"

// external
#include <magic_enum.hpp>

namespace ltb::wgpu
{
namespace
{

auto device_lost_callback(
    WGPUDevice const* const    device,
    WGPUDeviceLostReason const reason,
    WGPUStringView const       message,
    void* const                userdata1,
    void* const                userdata2
) -> void
{
    utils::ignore( userdata1 );
    utils::ignore( userdata2 );

    spdlog::warn(
        "WebGPU device ({}) lost ({}): {}",
        fmt::ptr( device ),
        magic_enum::enum_name( reason ),
        message.data
    );
}

auto error_callback(
    WGPUDevice const* const device,
    WGPUErrorType const     type,
    WGPUStringView const    message,
    void* const             userdata1,
    void* const             userdata2
) -> void
{
    utils::ignore( userdata1 );
    utils::ignore( userdata2 );

    spdlog::error(
        "WebGPU device ({}) uncaught error ({}): {}",
        fmt::ptr( device ),
        magic_enum::enum_name( type ),
        message.data
    );
}

struct DestroyInstance
{
    auto operator( )( WGPUInstanceImpl* const instance ) const -> void
    {
        if ( instance )
        {
            spdlog::info( "Destroying WGPU instance: {}", fmt::ptr( instance ) );
            ::wgpuInstanceRelease( instance );
        }
    }
};

struct DestroyAdapter
{
    auto operator( )( WGPUAdapterImpl* const adapter ) const -> void
    {
        if ( adapter )
        {
            spdlog::info( "Destroying WGPU adapter: {}", fmt::ptr( adapter ) );
            ::wgpuAdapterRelease( adapter );
        }
    }
};

struct DestroyDevice
{
    auto operator( )( WGPUDeviceImpl* const device ) const -> void
    {
        if ( device )
        {
            spdlog::info( "Destroying WGPU device: {}", fmt::ptr( device ) );
            ::wgpuDeviceRelease( device );
        }
    }
};

struct DestroyQueue
{
    auto operator( )( WGPUQueueImpl* const queue ) const -> void
    {
        if ( queue )
        {
            spdlog::info( "Destroying WGPU queue: {}", fmt::ptr( queue ) );
            ::wgpuQueueRelease( queue );
        }
    }
};

} // namespace

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
        instance_ = std::shared_ptr< WGPUInstanceImpl >( instance, DestroyInstance{ } );
    }
    else
    {
        spdlog::error( "Could not initialize WebGPU!" );
        return;
    }

    constexpr auto options = WGPURequestAdapterOptions{ };
    utils::ignore(
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
        )
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
    WGPUAdapterImpl* const         adapter,
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
    auto* app = static_cast< App* >( userdata1 );

    spdlog::info( "WebGPU adapter: {}", fmt::ptr( adapter ) );
    app->adapter_ = std::shared_ptr< WGPUAdapterImpl >( adapter, DestroyAdapter{ } );

    auto limits = WGPULimits{ };

    if ( WGPUStatus_Success == ::wgpuAdapterGetLimits( adapter, &limits ) )
    {
        spdlog::info(
            "Adapter limits:\n"
            " - maxTextureDimensions1D: {}\n"
            " - maxTextureDimensions2D: {}\n"
            " - maxTextureDimensions3D: {}\n"
            " - maxTextureArrayLayers: {}",
            limits.maxTextureDimension1D,
            limits.maxTextureDimension2D,
            limits.maxTextureDimension3D,
            limits.maxTextureArrayLayers
        );
    }

    auto features = WGPUSupportedFeatures{ };
    ::wgpuAdapterGetFeatures( adapter, &features );

    spdlog::info( "Adapter features ({}):", features.featureCount );
    for ( auto i = 0UL; i < features.featureCount; ++i )
    {
        auto const& feature = features.features[ i ];
        spdlog::info( " - {} ({:x})", to_string( feature ), std::to_underlying( feature ) );
    }

    auto info = WGPUAdapterInfo{ };
    if ( WGPUStatus_Success == ::wgpuAdapterGetInfo( adapter, &info ) )
    {
        spdlog::info(
            "Adapter info:\n"
            " - vendorID: {}\n"
            " - vendor: {}\n"
            " - architecture: {}\n"
            " - deviceID: {}\n"
            " - description: {}\n"
            " - adapterType: {}\n"
            " - backendType: {}",
            info.vendorID,
            info.vendor.data,
            info.architecture.data,
            info.deviceID,
            info.description.data,
            magic_enum::enum_name( info.adapterType ),
            magic_enum::enum_name( info.backendType )
        );
    }

    constexpr auto descriptor = WGPUDeviceDescriptor{
        .nextInChain          = nullptr,
        .label                = { },
        .requiredFeatureCount = 0UZ,
        .requiredFeatures     = nullptr,
        .requiredLimits       = nullptr,
        .defaultQueue         = { },
        .deviceLostCallbackInfo
        = { .nextInChain = nullptr,
            .mode        = WGPUCallbackMode_AllowProcessEvents,
            .callback    = device_lost_callback,
            .userdata1   = nullptr,
            .userdata2   = nullptr },
        .uncapturedErrorCallbackInfo
        = { .nextInChain = nullptr,
            .callback    = error_callback,
            .userdata1   = nullptr,
            .userdata2   = nullptr },
    };
    utils::ignore(
        ::wgpuAdapterRequestDevice(
            adapter,
            &descriptor,
            {
                .nextInChain = nullptr,
                .mode        = WGPUCallbackMode_AllowProcessEvents,
                .callback    = &App::handle_device,
                .userdata1   = app,
                .userdata2   = nullptr,
            }
        )
    );
}

auto App::handle_device(
    WGPURequestDeviceStatus const status,
    WGPUDeviceImpl* const         device,
    WGPUStringView const          message,
    void* const                   userdata1,
    void* const                   userdata2
) -> void
{
    utils::ignore( userdata2 );

    if ( WGPURequestDeviceStatus_Success != status )
    {
        spdlog::error( "Could not get WebGPU device: {}", message.data );
        return;
    }

    // Manipulate user data
    auto* app = static_cast< App* >( userdata1 );

    spdlog::info( "WebGPU device: {}", fmt::ptr( device ) );
    app->device_ = std::shared_ptr< WGPUDeviceImpl >( device, DestroyDevice{ } );

    auto limits = WGPULimits{ };

    if ( WGPUStatus_Success == ::wgpuDeviceGetLimits( device, &limits ) )
    {
        spdlog::info(
            "Device limits:\n"
            " - maxTextureDimensions1D: {}\n"
            " - maxTextureDimensions2D: {}\n"
            " - maxTextureDimensions3D: {}\n"
            " - maxTextureArrayLayers: {}",
            limits.maxTextureDimension1D,
            limits.maxTextureDimension2D,
            limits.maxTextureDimension3D,
            limits.maxTextureArrayLayers
        );
    }

    if ( auto* queue = ::wgpuDeviceGetQueue( device ) )
    {
        spdlog::info( "WebGPU queue: {}", fmt::ptr( queue ) );
        app->queue_ = std::shared_ptr< WGPUQueueImpl >( queue, DestroyQueue{ } );
    }
    else
    {
        spdlog::error( "Could not get WebGPU queue" );
    }

    if ( app->app_callback_ )
    {
        app->app_callback_( *app );
    }
}

} // namespace ltb::wgpu
