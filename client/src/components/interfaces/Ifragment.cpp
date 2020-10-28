/*
** EPITECH PROJECT, 2021
** R-type
** File description:
** R-type
*/

#include "components/window.hpp"
#include "components/interfaces/Ifragment.hpp"

void Ifragment::set_intent(const std::string &view)
{
    intent.emplace(view);
}

Ifragment::Ifragment(std::optional<std::string> &view_intent, sf::RenderWindow &main_window) :
        intent(view_intent), window(main_window)
{}

Ifragment::~Ifragment()
{
    for (auto &fragment : fragments) {
        delete fragment;
    }
}

void Ifragment::runCreate()
{
    auto center = sf::Vector2<float>(width / 2, height / 2);
    const auto parent_view = window.getView();
    const auto parent_viewport = parent_view.getViewport();
    const auto parent_size = parent_view.getSize();
    // POSITION
    const auto child_relative_x = 1 / (window::WIDTH / x);
    const auto child_relative_y = 1 / (window::HEIGHT / y);
    auto posx_percent = parent_viewport.left;
    auto posy_percent = parent_viewport.top;
    if (child_relative_x < 0)
        center.x += std::abs(x);
    else
        posx_percent += child_relative_x;
    if (child_relative_y < 0)
        center.y += std::abs(y);
    else
        posy_percent += child_relative_y;
    // SIZE
    auto final_width = width;
    auto final_height = height;
    if (x + width > parent_size.x) {
        const auto diff = x + width - parent_size.x;
        final_width -= diff;
        center.x -= diff;
    }
    if (y + height > parent_size.y) {
        const auto diff = y + height - parent_size.y;
        final_height -= diff;
        center.y -= diff;
    }
    auto width_percent = 1 / (window::WIDTH / final_width);                  // TODO parent_offset.width
    auto height_percent = 1 / (window::HEIGHT / final_height);               // TODO parent_offset.height
    // BUILD VIEW
    content.setCenter(center);
    content.setSize(final_width, final_height);
    content.setViewport(sf::FloatRect(posx_percent, posy_percent, width_percent, height_percent));

    // background
    background.setFillColor(background_color);
    background.setSize(sf::Vector2f(width, height));
    background.setPosition(0, 0);
    // sort z_index
    std::sort(fragments.begin(), fragments.end(), [](const auto &left, const auto &right) {
        return left->z_index < right->z_index;
    });
    onCreateView();
    window.setView(content);
    for (auto &fragment : fragments) {
        fragment->runCreate();
    }
    window.setView(parent_view);
}

void Ifragment::runUpdate()
{
    window.setView(content);
    window.draw(background);
    onUpdateView();
    for (auto &fragment : fragments) {
        fragment->runUpdate();
    }
}

void Ifragment::runFinish()
{
    for (auto &fragment : fragments) {
        fragment->runFinish();
    }
    onFinishView();
}
