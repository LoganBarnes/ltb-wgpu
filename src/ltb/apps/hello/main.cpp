// ///////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Logan Barnes - All Rights Reserved
// ///////////////////////////////////////////////////////////////////////////////////////

// external
#include <spdlog/spdlog.h>
#include <webgpu/webgpu.h>

// standard
#include <iostream>

int main( )
{
    // We create a descriptor
    auto desc = WGPUInstanceDescriptor{ };

    // We create the instance using this descriptor
#ifdef WEBGPU_BACKEND_EMSCRIPTEN
    auto* instance = ::wgpuCreateInstance( nullptr );
#else
    auto* instance = ::wgpuCreateInstance( &desc );
#endif

    // We can check whether there is actually an instance created
    if ( !instance )
    {
        spdlog::error( "Could not initialize WebGPU!" );
        return EXIT_FAILURE;
    }

    // Display the object (WGPUInstance is a simple pointer, it may be
    // copied around without worrying about its size).
    spdlog::info( "WGPU instance: {}", fmt::ptr( instance ) );

    // We clean up the WebGPU instance
    ::wgpuInstanceRelease( instance );

    return EXIT_SUCCESS;
}
