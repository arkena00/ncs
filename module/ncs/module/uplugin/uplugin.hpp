#pragma once

#include <ncs/module.hpp>
#include <ncs/parameter.hpp>
#include <ncs/module/uplugin/commands.hpp>
#include <ncs/module/uplugin/commands_impl.hpp>

namespace ncs::modules
{
    class uplugin : public ncs::module
    {
    public:
        explicit uplugin(ncs::core& core)
            : ncs::module(core, "uplugin")
            , core_{ core }
            , commands_impl_{ *this }
            , uplugin_commands_{ commands_impl_ }
            , commands_node_{ uplugin_commands_ }
        {

            //core_.add_commands(commands_);
        }

        /*
        void make(const ncs::parameters& input)
        {
            mod::generator generator{ core_, "$ncs" };
            generator.process(input);
        }*/

        ncs::core& core() { return core_; }

        [[nodiscard]] ncs::commands_base& commands() override { return commands_node_.get_commands(); }

    private:
        ncs::core& core_;
        ncs::internal::modules::uplugin::uplugin_commands uplugin_commands_;
        ncs::internal::modules::uplugin::commands_node commands_node_;
        ncs::internal::modules::uplugin::commands_impl commands_impl_;
    };
} // ncs