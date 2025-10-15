// ///////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Logan Barnes - All Rights Reserved
// ///////////////////////////////////////////////////////////////////////////////////////
#include "ltb/window/glfw_utils.hpp"

// external
#include <spdlog/spdlog.h>

/// \todo: replace glfw* functions with a mockable GLFW interface.

namespace ltb::window
{
namespace
{

auto apply_window_hints( WindowSettings const& settings ) -> utils::Result< void >
{
    ::glfwWindowHint( GLFW_VISIBLE, GLFW_TRUE );

    if ( settings.transparent_background )
    {
        ::glfwWindowHint( GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_TRUE );
    }
    else
    {
        ::glfwWindowHint( GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_FALSE );
    }

    if ( settings.title_bar )
    {
        ::glfwWindowHint( GLFW_DECORATED, GLFW_TRUE );
    }
    else
    {
        ::glfwWindowHint( GLFW_DECORATED, GLFW_FALSE );
    }

    if ( settings.resizable )
    {
        ::glfwWindowHint( GLFW_RESIZABLE, GLFW_TRUE );
    }
    else
    {
        ::glfwWindowHint( GLFW_RESIZABLE, GLFW_FALSE );
    }

    ::glfwWindowHint( GLFW_CLIENT_API, GLFW_NO_API );

    return utils::success( );
}

auto get_monitor( ) -> utils::Result< GLFWmonitor* >
{
    auto         monitor_count = int32{ 0 };
    auto** const monitors      = ::glfwGetMonitors( &monitor_count );
    if ( nullptr == monitors )
    {
        return LTB_MAKE_UNEXPECTED_ERROR( "glfwGetMonitors() failed" );
    }
    if ( monitor_count < 1 )
    {
        return LTB_MAKE_UNEXPECTED_ERROR( "No monitors found" );
    }

    constexpr auto default_monitor_index = uint32{ 0 };

    auto* const monitor = monitors[ default_monitor_index ];
    if ( nullptr == monitor )
    {
        return LTB_MAKE_UNEXPECTED_ERROR( "Monitor not found" );
    }

    return monitor;
}

auto get_initial_size(
    GLFWmonitor* const                 monitor,
    bool const                         title_bar,
    std::optional< glm::uvec2 > const& requested_initial_size
) -> utils::Result< glm::ivec2 >
{
    if ( nullptr == monitor )
    {
        return LTB_MAKE_UNEXPECTED_ERROR( "Null monitor provided" );
    }

    auto const* const video_mode = ::glfwGetVideoMode( monitor );
    if ( nullptr == video_mode )
    {
        return LTB_MAKE_UNEXPECTED_ERROR( "glfwGetVideoMode() failed" );
    }
    ::glfwWindowHint( GLFW_RED_BITS, video_mode->redBits );
    ::glfwWindowHint( GLFW_GREEN_BITS, video_mode->greenBits );
    ::glfwWindowHint( GLFW_BLUE_BITS, video_mode->blueBits );
    ::glfwWindowHint( GLFW_REFRESH_RATE, video_mode->refreshRate );

    auto initial_size = glm::ivec2{ };

    // A specific size was requested.
    if ( requested_initial_size.has_value( ) )
    {
        initial_size = glm::ivec2( requested_initial_size.value( ) );
    }
    // Full screen with title bar.
    else if ( title_bar )
    {
        initial_size = glm::ivec2{ video_mode->width, video_mode->height };
    }
    // Full screen without the title bar.
    else
    {
        // Get the work area of the monitor to create a window that fits
        // the screen without overlapping the taskbar or other system UI.
        struct WorkArea
        {
            glm::ivec2 position = { };
            glm::ivec2 size     = { };
        };

        auto work_area = WorkArea{ };

        ::glfwGetMonitorWorkarea(
            monitor,
            &work_area.position.x,
            &work_area.position.y,
            &work_area.size.x,
            &work_area.size.y
        );

        initial_size = work_area.size;
    }

    return initial_size;
}

auto default_glfw_error_callback( int32_t const error, char const* const description ) -> void
{
    spdlog::error( "GLFW Error ({}): {}", error, description );
}

auto glfw_framebuffer_size_callback(
    GLFWwindow* const window,
    int32_t const     width,
    int32_t const     height
) -> void
{
    auto* const callback_data
        = static_cast< CallbackData* >( ::glfwGetWindowUserPointer( window ) );
    callback_data->resized_framebuffer = glm::ivec2{ width, height };
}

} // namespace

ScopedGlfw::ScopedGlfw( )
    : init_value{ ::glfwInit( ) }
{
}

ScopedGlfw::~ScopedGlfw( )
{
    if ( GLFW_FALSE != init_value )
    {
        ::glfwTerminate( );
    }
}

auto GlfwWindowDeleter::operator( )( GLFWwindow* const window ) const -> void
{
    if ( nullptr != window )
    {
        ::glfwDestroyWindow( window );
    }
}

auto initialize_glfw( WindowSettings const& settings )
    -> utils::Result< std::tuple< GlfwHandle, GlfwWindowHandle > >
{

    // Set the error callback before any GLFW calls to log when things go wrong.
    // The previous callback is ignored because it doesn't need to be restored.
    utils::ignore( ::glfwSetErrorCallback( default_glfw_error_callback ) );

    // Initialize the window framework library.
    auto glfw = std::make_unique< ScopedGlfw >( );
    if ( GLFW_FALSE == glfw->init_value )
    {
        return LTB_MAKE_UNEXPECTED_ERROR( "glfwInit() failed" );
    }

    // Apply window settings.
    LTB_CHECK( apply_window_hints( settings ) );
    LTB_CHECK( auto* const monitor, get_monitor( ) );
    LTB_CHECK(
        auto const initial_size,
        get_initial_size( monitor, settings.title_bar, settings.initial_size )
    );

    // Create the window.
    auto window = GlfwWindowHandle{
        ::glfwCreateWindow(
            initial_size.x,
            initial_size.y,
            settings.title.data( ),
            nullptr,
            nullptr
        ),
    };
    if ( nullptr == window )
    {
        return LTB_MAKE_UNEXPECTED_ERROR( "glfwCreateWindow() failed" );
    }

    if ( settings.initial_position.has_value( ) )
    {
        auto monitor_pos = glm::ivec2{ };
        ::glfwGetMonitorPos( monitor, &monitor_pos.x, &monitor_pos.y );

        monitor_pos += settings.initial_position.value( );
        ::glfwSetWindowPos( window.get( ), monitor_pos.x, monitor_pos.y );
    }

    return std::make_tuple( std::move( glfw ), std::move( window ) );
}

auto set_callbacks( GLFWwindow* window ) -> utils::Result< std::unique_ptr< CallbackData > >
{
    auto callback_data = std::make_unique< CallbackData >( );

    ::glfwSetWindowUserPointer( window, callback_data.get( ) );

    // Get the current framebuffer size for any graphics APIs using the window.
    auto framebuffer_size = glm::ivec2{ };

    ::glfwGetFramebufferSize( window, &framebuffer_size.x, &framebuffer_size.y );
    callback_data->resized_framebuffer = framebuffer_size;

    // Ignore the old, returned callback.
    utils::ignore( ::glfwSetFramebufferSizeCallback( window, glfw_framebuffer_size_callback ) );

    return callback_data;
}

} // namespace ltb::window
