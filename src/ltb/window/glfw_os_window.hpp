// ///////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Logan Barnes - All Rights Reserved
// ///////////////////////////////////////////////////////////////////////////////////////
#pragma once

// project
#include "ltb/window/glfw_utils.hpp"
#include "ltb/window/os_window.hpp"

namespace ltb::window
{

/// \brief An operating system window created with the GLFW framework.
class GlfwOsWindow : public OsWindow
{
public:
    explicit GlfwOsWindow( WindowSettings settings );
    ~GlfwOsWindow( ) override;

    auto initialize( ) -> utils::Result< void > override;

    [[nodiscard( "Const getter" )]] auto is_initialized( ) const -> bool override;

    auto poll_events( ) -> void override;

    [[nodiscard( "Const getter" )]] auto should_close( ) const -> bool override;

    [[nodiscard( "Const getter" )]] auto resized( ) const -> std::optional< glm::ivec2 > override;

    /// \brief The raw GLFW window handle. This will be null if
    ///        the window is was not initialized successfully.
    auto glfw_window( ) -> GLFWwindow*;

private:
    WindowSettings const settings_;

    /// \brief RAII object to handle a GLFW context.
    GlfwHandle glfw_ = nullptr;

    /// \brief RAII object to handle a GLFW window.
    GlfwWindowHandle window_ = nullptr;

    // CallbackData is its own struct and stored as a smart pointer because GLFW
    // requires a raw pointer to the data. If callback_data was stored as a local
    // member variable and this class was moved, the pointer would become invalid.
    std::unique_ptr< CallbackData > callback_data_ = nullptr;

    bool initialized_ = false;
};

} // namespace ltb::window
