/*
** EPITECH PROJECT, 2021
** R-type
** File description:
** R-type
*/

#include "sdk/widgets/text_entry.hpp"

WidgetText_entry::WidgetText_entry(std::optional<std::string> &view_intent, bidimensional::Transform &parent_trans, sf::RenderWindow &main_window, Itheme<Icolors *> *theme) :
    Iwidget(view_intent, parent_trans, main_window)
{
    add_widget<WidgetText>("text", theme);
    text = get_fragment<WidgetText>("text");
    scale({100, 12});
    cursor_clock.restart();
    reload();
}

bool WidgetText_entry::is_hover()
{
    auto mouse = Input::getMouse();
    if (mouse.x < transform.position.x || mouse.x > transform.position.x + transform.scale.x)
        return false;
    if (mouse.y < transform.position.y || mouse.y > transform.position.y + transform.scale.y)
        return false;
    return true;
}

bool WidgetText_entry::is_clicked()
{
    return is_hover() && Input::getKeys(keyboard::LeftClick) == keyboard::KeyStatus::PRESSED;
}

void WidgetText_entry::onCreateView()
{}

#include <iostream>

void WidgetText_entry::onUpdateView()
{
    if (is_clicked())
        isfocus = true;
    else if (!is_hover() && Input::getKeys(keyboard::LeftClick) == keyboard::KeyStatus::PRESSED)
        isfocus = false;
    if (isfocus) {
        bool has_enter_or_delete = false;
        auto key_queue = Input::getKeysQueue();
        for (auto i = 0; i < key_queue.size; ++i) {
            if (key_queue.data[i].pressed) {
                // delete
                if (key_queue.data[i].key == keyboard::Key::BackSpace) {
                    has_enter_or_delete = true;
                    if (!data.empty())
                        data.pop_back();
                    continue;
                }
                // enter
                if (key_queue.data[i].key == keyboard::Key::Enter) {
                    has_enter_or_delete = true;
                    handler(data);
                    clear();
                    continue;
                }
            }
        }
        if (has_enter_or_delete) {
            reload();
        } else {
            auto text_entered = Input::get_text_entered();
            if (!text_entered.empty()) {
                data += text_entered;
                reload();
            }
        }
        // draw cursor
        if (cursor_clock.getElapsedTime().asMilliseconds() >= cursor_timeout) {
            std::cout << iscursor << std::endl;
            cursor_clock.restart();
            iscursor = !iscursor;
            if (iscursor) {
                cursor = "|";
            } else {
                cursor = " ";
            }
        }
    } else {
        if (iscursor) {
            iscursor = false;
            cursor.clear();
        }
    }
    if (!cursor.empty() && !data.empty())
        text->set_text(data + cursor);
    if (data.empty() && !is_placeholdermode) {
        is_placeholdermode = true;
        reload();
    }
    if (!data.empty() && is_placeholdermode) {
        is_placeholdermode = false;
        reload();
    }
}

void WidgetText_entry::onFinishView()
{}

void WidgetText_entry::reload()
{
    if (is_placeholdermode) {
        text->set_text(placeholder);
        text->set_color(placeholder_color);
    } else {
        text->set_color(textcolor);
        text->set_text(data);
    }
}

void WidgetText_entry::clear()
{
    data.clear();
    reload();
}

void WidgetText_entry::set_font(const std::string &path)
{
    text->set_font(path);
}

void WidgetText_entry::set_fontsize(unsigned int newsize)
{
    text->set_fontsize(newsize);
}

void WidgetText_entry::set_color(sf::Color color)
{
    textcolor = color;
    text->set_color(textcolor);
}

void WidgetText_entry::set_background(sf::Color color)
{
    background_color = color;
}

void WidgetText_entry::set_handler(std::function<void(std::string_view)> functor)
{
    handler = functor;
}

void WidgetText_entry::set_placeholder(const std::string &str)
{
    placeholder = str;
    reload();
}

void WidgetText_entry::set_placeholder_color(sf::Color color)
{
    placeholder_color = color;
    reload();
}




