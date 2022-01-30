#pragma once

#include <chrono>
#include <cmath>
#include <stack>
#include <string>

#include "SFML/Graphics/PrimitiveType.hpp"
#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/Graphics/VertexArray.hpp"
#include "SFML/Graphics/View.hpp"
#include "imgui.h"

#include "LSystem.h"

static const sf::Color color_palette[] = {sf::Color::Cyan,    sf::Color::Blue, sf::Color::Green,
                                          sf::Color::Magenta, sf::Color::Red,  sf::Color::Yellow};
constexpr float PI = 3.1415926535f;

struct TurtleStatus {
    TurtleStatus() : pos{0, 0}, angle{0}, color{0} {}

    TurtleStatus(const TurtleStatus& other) : pos{other.pos}, angle{other.angle}, color{other.color}
    {}

    sf::Vector2f pos;
    float angle;
    size_t color;
};

class LSystemRenderer {
public:
    LSystemRenderer();

    void updateBuffers();
    void update();
    void configWindow(sf::View& view);

    void draw(sf::RenderWindow& win, sf::View& view);

private:
    void executeLSystemStack(const std::string& lsys, float angle_step, float dist_step);

    void executeLSystemBranch(const std::string& lsys, size_t& index, sf::Vector2f pos, float angle,
                              float angle_step, float dist_step, size_t color);
    void execLSystem(const std::string& lsys, float angle_step, float dist_step);

private:
    static const size_t BUFSIZE = 128;
    static const size_t NUM_RULES = 5;
    char mAxiomBuffer[BUFSIZE];
    char mRuleBuffers[NUM_RULES][BUFSIZE];

    LSystem mLsystem;

    int mGeneration;
    int mAngle;
    sf::VertexArray mVertices;
};