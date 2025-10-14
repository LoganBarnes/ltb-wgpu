// ///////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Logan Barnes - All Rights Reserved
// ///////////////////////////////////////////////////////////////////////////////////////

// project
#include "ltb/utils/types.hpp"
#include "ltb/wgpu/app.hpp"

// standard
#include "ltb/window/glfw_os_window.hpp"

#include <atomic>

int main( )
{
    spdlog::set_level( spdlog::level::debug );

    auto window = ltb::window::GlfwOsWindow{ {
        .title        = "Hello",
        .initial_size = glm::ivec2{ 1280, 720 },
    } };

    auto app = ltb::wgpu::App{ { .window = &window } };

    app.run( );

    spdlog::debug( "Waiting..." );
    while ( !window.should_close( ) )
    {
        window.poll_events( );
        app.process( );
    }
    spdlog::debug( "Exiting." );

    return EXIT_SUCCESS;
}
