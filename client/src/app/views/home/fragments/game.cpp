/*
** EPITECH PROJECT, 2021
** R-type
** File description:
** R-type
*/

#include "app/res/theme.hpp"
#include "app/views/home/fragments/topbar.hpp"
#include "app/views/home/fragments/bottombar.hpp"
#include "app/views/home/fragments/game.hpp"
#include "app/window.hpp"

Game::Game(std::optional<std::string> &intent_ref, bidimensional::Transform &parent_trans, sf::RenderWindow &window) :
        Ifragment(intent_ref, parent_trans, window)
{
    transform.position = {0, TopBar::bar_height};
    transform.scale = {window::WIDTH, window::HEIGHT - TopBar::bar_height - BottomBar::bar_height};
    background_color = Theme().getColor("Transparent").value(); ;
    z_index = 1;
    client.set_onDisconnect([&](){
        is_connect = false;
//        sprites.clear();
    });
    main_texture.create(1800, 960);
    main_sprite.setTexture(main_texture.getTexture());
    main_sprite.setOrigin(0, 0);
    main_sprite.setPosition(0, 0);
}

void Game::onCreateView()
{
    client.setConsole(console);

}

void Game::onUpdateView()
{
    try {
        client.update();
    } catch (std::exception e) {
        console->log("EXCEPTION : " + std::string(e.what()));
        disconnect();
    }
    if (is_connect) {
        window.draw(main_sprite);
    }
//    if (is_connect) {
//        auto new_sprites = client.getSprites();
//        if (!new_sprites.empty()) {
//            sprites = new_sprites;
//        }
//        if (sprites.size()) {
//            for (int i = sprites.size() - 1; i >= 0 ; --i) {
//                window.draw(sprites[i]);
//            }
//        }
//    }
}

void Game::onFinishView()
{
    if (is_connect)
        disconnect();
}

bool *Game::get_is_connect()
{
    return &is_connect;
}

void Game::disconnect()
{
    client.disconnect();
    client.reset();         // TODO necessary ?
    set_intent("loading");
}

void Game::connect(const std::string &address)
{
    if (is_connect)
        disconnect();
    is_connect = true;
    client.connect(address);
}

void Game::set_console(Console *newconsole)
{
    console = newconsole;
}
