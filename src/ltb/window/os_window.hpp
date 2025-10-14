// ///////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Logan Barnes - All Rights Reserved
// ///////////////////////////////////////////////////////////////////////////////////////
#pragma once

// project
#include "ltb/utils/result.hpp"

// external
#include <glm/glm.hpp>
#include <webgpu/webgpu.h>

// standard
#include <optional>

namespace ltb::window
{

/// \brief An abstract window interface for the operating system.
class OsWindow
{
public:
    virtual ~OsWindow( ) = 0;

    /// \brief Initializes an operating system window to which a graphics API can render images.
    virtual auto initialize( ) -> utils::Result< void > = 0;

    /// \brief Returns true if the window has been initialized.
    [[nodiscard( "Const getter" )]] virtual auto is_initialized( ) const -> bool = 0;

    /// \brief Returns the WebGPU surface for the window.
    [[nodiscard( "Const getter" )]]
    virtual auto get_surface( WGPUInstanceImpl* instance ) -> WGPUSurface
        = 0;

    /// \brief Checks for input and resize events from the window.
    virtual auto poll_events( ) -> void = 0;

    /// \brief Returns true if a window close event was requested.
    [[nodiscard( "Const getter" )]] virtual auto should_close( ) const -> bool = 0;

    /// \brief Returns the new size of the framebuffer if the window was resized.
    ///        This should always return a value immediately after initialization.
    [[nodiscard( "Const getter" )]] virtual auto resized( ) const -> std::optional< glm::ivec2 >
        = 0;
};

inline OsWindow::~OsWindow( ) = default;

} // namespace ltb::window
