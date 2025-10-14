// ///////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Logan Barnes - All Rights Reserved
// ///////////////////////////////////////////////////////////////////////////////////////

// project
#include "ltb/utils/types.hpp"
#include "ltb/wgpu/app.hpp"

// standard
#include <atomic>

namespace ltb
{
namespace
{

constexpr auto default_flag = uint8{ 1 };
constexpr auto exit_flag    = uint8{ 2 };

struct ExitCallback
{
    std::atomic_uchar& signal_flag_;

    auto operator( )( wgpu::App& app ) const
    {
        utils::ignore( app );
        signal_flag_.store( exit_flag );
    }
};

} // namespace
} // namespace ltb

int main( )
{
    spdlog::set_level( spdlog::level::debug );

    auto signal_flag = std::atomic_uchar{ ltb::default_flag };

    auto app = ltb::wgpu::App{ ltb::ExitCallback{ signal_flag } };

    app.run( );

    spdlog::debug( "Waiting..." );
    while ( ltb::exit_flag != signal_flag.load( ) )
    {
        app.process( );
    }
    spdlog::debug( "Exiting." );

    return EXIT_SUCCESS;
}
