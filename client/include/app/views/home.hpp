/*
** EPITECH PROJECT, 2021
** R-type
** File description:
** R-type
*/

#ifndef _HOME_HPP_
#define _HOME_HPP_

#include "sdk/interfaces/Iview.hpp"

class HomeView : public Iview {
    void onCreateView() final;
    void onUpdateView() final;
    void onFinishView() final;
    sf::Font helvetica;
    sf::Text text;
    sf::Clock clock;
    unsigned fps = 0;
public:
    explicit HomeView(sf::RenderWindow &window);
};

#endif