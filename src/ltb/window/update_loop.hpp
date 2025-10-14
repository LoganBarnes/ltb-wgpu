// ///////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Logan Barnes - All Rights Reserved
// ///////////////////////////////////////////////////////////////////////////////////////
#pragma once
//
// // project
// // #include "ltb/framework/update_loop.hpp"
// // #include "ltb/framework/update_loop_data.hpp"
// #include "ltb/utils/ignore.hpp"
// #include "ltb/utils/result.hpp"
// #include "ltb/window/os_window.hpp"
//
// // standard
// #include <chrono>
// #include <concepts>
// #include <numeric>
//
// namespace ltb::window {
//
// template <typename Object>
// concept Updatable = requires(Object obj) {
//   { obj.initialize() } -> std::same_as<utils::Result<void>>;
//   { obj.updates() } -> std::same_as<std::vector<framework::PeriodicUpdate<>>>;
//   { obj.clean_up() } -> std::same_as<utils::Result<void>>;
// };
//
// /// \brief Run a loop that will run as fast as possible.
// template <typename ResizeCallback, typename Clock = std::chrono::steady_clock>
// auto run_non_blocking_update_loop(window::IOsWindow&                                   window,
//                                   ResizeCallback const&                                on_resize,
//                                   std::vector<framework::PeriodicUpdate<Clock>> const& updates)
//   -> void {
//
//   // Initialize the timing data for all updates.
//   auto updates_timing
//     = std::vector<framework::PeriodicUpdateWithTimer<Clock>>(updates.begin(), updates.end());
//
//   // Loop until the user requests to exit the update loop.
//   while (!window.should_close()) {
//     window.poll_events();
//
//     if (auto const new_size = window.resized()) {
//       on_resize(new_size.value());
//     }
//
//     // Invoke any updates that are past due
//     for (auto& timing : updates_timing) {
//       timing.run_if_overdue(Clock::now());
//     }
//
//     window.swap_buffers();
//   }
// }
//
// template <typename App>
// requires Updatable<App>
//
// auto run_non_blocking(window::IOsWindow& window, App& app) -> utils::Result<void> {
//   LTB_CHECK(window.initialize());
//   LTB_CHECK(app.initialize());
//
//   if (auto const new_size = window.resized()) {
//     app.resize(new_size.value());
//   }
//
//   run_non_blocking_update_loop(window, std::bind_front(&App::resize, &app), app.updates());
//
//   return app.clean_up();
// }
//
// }  // namespace ltb::window
