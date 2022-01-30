#include <array>
#include <chrono>
#include <cmath>
#include <iostream>
#include <stack>
#include <string>
#include <vector>

#include <SFML/Graphics.hpp>
#include <imgui-SFML.h>
#include <imgui.h>

#include "LSystem.h"
#include "LSystemRenderer.h"

#ifdef _WIN32
#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
#endif

sf::Vector2f getMouseWorld(sf::RenderWindow& win)
{
    return win.mapPixelToCoords(sf::Mouse::getPosition(win));
}

void zoomViewAt(sf::Vector2i pixel, sf::View& view, sf::RenderWindow& window, float zoom)
{
    const sf::Vector2f beforeCoord{window.mapPixelToCoords(pixel)};
    view.zoom(zoom);
    window.setView(view);
    const sf::Vector2f afterCoord{window.mapPixelToCoords(pixel)};
    const sf::Vector2f offsetCoords{beforeCoord - afterCoord};
    view.move(offsetCoords);
}

int main()
{
    sf::RenderWindow window(sf::VideoMode(1600, 900), "LSystem");
    window.setFramerateLimit(60);

    ImGui::SFML::Init(window);

    sf::View view;
    view.setCenter(0, 0);
    view.setSize(window.getSize().x, window.getSize().y);
    view.rotate(90);

    // sf::FloatRect bounds = lshape.getBounds();
    // view.setCenter(bounds.left + bounds.width / 2, bounds.top + bounds.height / 2);
    // view.setSize(bounds.width, bounds.height);

    LSystemRenderer lsysrenderer{};

    float zoom = 1;
    bool mouse_pressed = false;
    sf::Vector2f mouseOld{0, 0};

    while (window.isOpen()) {
        sf::Event event;
        sf::Clock deltaClock;

        while (window.pollEvent(event)) {
            ImGui::SFML::ProcessEvent(event);
            if (event.type == sf::Event::Closed) window.close();

            if (event.type == sf::Event::Resized) {
                view.setSize(event.size.width, event.size.height);
            }

            if (ImGui::GetIO().WantCaptureMouse) {
                mouse_pressed = false;
                continue;
            }

            if (event.type == sf::Event::MouseWheelScrolled) {
                if (event.mouseWheelScroll.delta > 0) {
                    zoomViewAt({event.mouseWheelScroll.x, event.mouseWheelScroll.y}, view, window,
                               0.9f);
                } else {
                    zoomViewAt({event.mouseWheelScroll.x, event.mouseWheelScroll.y}, view, window,
                               1.1f);
                }
            }

            if (event.type == sf::Event::MouseMoved && mouse_pressed) {
                auto mouseWorld = getMouseWorld(window);
                auto offset = mouseOld - mouseWorld;
                view.move(offset);
                window.setView(view);
                mouseOld = getMouseWorld(window);
            }

            if (event.type == sf::Event::MouseButtonPressed &&
                event.mouseButton.button == sf::Mouse::Left) {
                mouseOld = getMouseWorld(window);
                mouse_pressed = true;
            }

            if (event.type == sf::Event::MouseButtonReleased &&
                event.mouseButton.button == sf::Mouse::Left)
                mouse_pressed = false;
        }

        ImGui::SFML::Update(window, deltaClock.restart());

        // std::printf("%f %f; %f %f\n", view.getCenter().x, view.getCenter().y, view.getSize().x,
        // view.getSize().y);
        window.clear();
        window.setView(view);

        lsysrenderer.draw(window, view);
        ImGui::SFML::Render(window);
        window.display();
    }

    return 0;
}
