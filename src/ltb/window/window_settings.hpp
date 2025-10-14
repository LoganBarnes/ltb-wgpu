// ///////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Logan Barnes - All Rights Reserved
// ///////////////////////////////////////////////////////////////////////////////////////
#pragma once

// external
#include <glm/glm.hpp>

// standard
#include <optional>
#include <string>

namespace ltb::window
{

/// \brief Settings used to initialize an operating system window.
struct WindowSettings
{
    /// \brief If true, the alpha channel of the window will be transparent.
    std::string title = "Window";

    /// \brief If true, the alpha channel of the window will be transparent.
    bool transparent_background = false;

    /// \brief If true, the window will be resizable.
    bool resizable = true;

    /// \brief If true, the window will contain a title_bar.
    bool title_bar = true;

    /// \brief The initial size of the window. Fullscreen if nullopt.
    std::optional< glm::ivec2 > initial_size = std::nullopt;

    /// \brief The initial position of the window. If nullopt, the window will
    ///        be positioned by the operating system.
    std::optional< glm::ivec2 > initial_position = std::nullopt;
};

} // namespace ltb::window
