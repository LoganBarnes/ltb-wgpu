// ///////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Logan Barnes - All Rights Reserved
// ///////////////////////////////////////////////////////////////////////////////////////
#include "ltb/window/glfw_os_window.hpp"

// project
#include "ltb/window/glfw_utils.hpp"

// external
#include <spdlog/spdlog.h>
#include <webgpu/webgpu_glfw.h>

namespace ltb::window
{

GlfwOsWindow::GlfwOsWindow( WindowSettings settings )
    : settings_{ std::move( settings ) }
{
}

GlfwOsWindow::~GlfwOsWindow( ) = default;

auto GlfwOsWindow::initialize( ) -> utils::Result< void >
{
    if ( is_initialized( ) )
    {
        return utils::success( );
    }

    LTB_CHECK( auto init_results, initialize_glfw( settings_ ) );
    auto& [ glfw, window ] = init_results;

    LTB_CHECK( auto callback_data, set_callbacks( window.get( ) ) );

    // No more errors can occur, so the resources can be moved.
    glfw_          = std::move( glfw );
    window_        = std::move( window );
    callback_data_ = std::move( callback_data );
    initialized_   = true;

    return utils::success( );
}

auto GlfwOsWindow::is_initialized( ) const -> bool
{
    return initialized_;
}

auto GlfwOsWindow::get_surface( WGPUInstanceImpl* const instance ) -> WGPUSurface
{
    return ::wgpuGlfwCreateSurfaceForWindow( instance, window_.get( ) );
}

auto GlfwOsWindow::poll_events( ) -> void
{
    if ( is_initialized( ) )
    {
        callback_data_->resized_framebuffer = std::nullopt;
        ::glfwPollEvents( );
    }
}

auto GlfwOsWindow::should_close( ) const -> bool
{
    if ( !is_initialized( ) )
    {
        spdlog::warn( "Window not initialized, suggesting it should close." );
        return true;
    }
    return ::glfwWindowShouldClose( window_.get( ) );
}

auto GlfwOsWindow::resized( ) const -> std::optional< glm::ivec2 >
{
    if ( !is_initialized( ) )
    {
        spdlog::warn( "Window not initialized. No resized framebuffer available." );
        return std::nullopt;
    }
    return callback_data_->resized_framebuffer;
}

auto GlfwOsWindow::glfw_window( ) -> GLFWwindow*
{
    return window_.get( );
}

} // namespace ltb::window
