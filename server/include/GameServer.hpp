#include <array>
#include <unordered_map>
#include <unordered_set>

#ifndef _GAMESERVER_HPP_
#define _GAMESERVER_HPP_

#include "Storage.hpp"
#include "Modules.hpp"
#include "interface/UdpServer.hpp"
#include "AssetsServer.hpp"

struct Player {
    bool ready = false;
    std::unordered_set<protocol::udp::keys::Keys> keys;
    Dimensional cur_pos = {0, 0};
};

class GameServer:
    public ecs::udp::Server<protocol::udp::RequestCode>,
    public IAudioModule,
    public IDisplayModule,
    public IInputModule
{
    public:
        GameServer(unsigned port, std::filesystem::path &p);
        ~GameServer();

        // Required by IModule
        virtual void update()final;

        // Required by ecs::udp::Server
        virtual void onMessage(Message<protocol::udp::RequestCode> msg)final;

        // Required by IAudioModule
        virtual void playSound(const std::string &name, float volume,
                               float pitch, bool looping) final;
        void stopSound(const std::string &name) override;

        // Required by IDisplayModule
        virtual void drawSprite(const std::string &name, Transform const &transf, unsigned tile_id)final;

        // Required by IInputModule
        virtual Dimensional getCursorLocation(const unsigned player)final;
        virtual bool isKeyPressed(unsigned player, Input key)final;

    private:
        AssetsServer assets;
        std::shared_ptr<Storage> stor;
        std::unordered_map<
            uint32_t,
            std::pair<std::shared_ptr<ecs::IClient<protocol::udp::RequestCode>>,
                      Player>>
            list;
        std::deque<protocol::udp::Sprite> pending_sprite;
        unsigned port;
};

#endif //_GAMESERVER_HPP_
