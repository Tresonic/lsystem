#include "LSystemRenderer.h"

LSystemRenderer::LSystemRenderer() :
    mLsystem({"F++F++F"}, {{'F', {"F-F++F-F"}}}, 60),
    mVertices(sf::Lines),
    mGeneration(5),
    mAngle(60)
{
    updateBuffers();
    update();
}

void LSystemRenderer::updateBuffers()
{
    strcpy(mAxiomBuffer, mLsystem.getAxiom().c_str());
    int i = 0;
    for (auto& rule : mLsystem.getRules()) {
        std::string rule_str = std::string(1, rule.first) + "=" + rule.second;
        strcpy(mRuleBuffers[i++], rule_str.c_str());
    }
    while (i < NUM_RULES) {
        strcpy(mRuleBuffers[i++], "");
    }
}

void LSystemRenderer::update()
{
    auto t1 = std::chrono::high_resolution_clock::now();
    std::string lsys = mLsystem.generate(mGeneration);
    auto t2 = std::chrono::high_resolution_clock::now();
    // auto ms_int = duration_cast<std::chrono::milliseconds>(t2 - t1);
    // std::cout << "lsys time: " << ms_int << '\n';

    t1 = std::chrono::high_resolution_clock::now();
    execLSystem(lsys, mAngle, 10);
    t2 = std::chrono::high_resolution_clock::now();
    // ms_int = duration_cast<std::chrono::milliseconds>(t2 - t1);
    // std::cout << "recursive: " << ms_int << '\n';

    t1 = std::chrono::high_resolution_clock::now();
    executeLSystemStack(lsys, mAngle, 10);
    t2 = std::chrono::high_resolution_clock::now();
    // ms_int = duration_cast<std::chrono::milliseconds>(t2 - t1);
    // std::cout << "stack: " << ms_int << '\n';

    // t1 = std::chrono::high_resolution_clock::now();
    // executeLSystemStack(lsys, mAngle, 10);
    // t2 = std::chrono::high_resolution_clock::now();
    // ms_int = duration_cast<std::chrono::milliseconds>(t2 - t1);
    // std::cout << "stack: " << ms_int << '\n';
}

void LSystemRenderer::configWindow(sf::View& view)
{
    static bool error = false;
    static std::string error_msg;

    ImGui::Begin("L-System", 0, ImGuiWindowFlags(ImGuiWindowFlags_AlwaysAutoResize));

    ImGui::Text("Generation");
    ImGui::SliderInt("##slidergeneration", &mGeneration, 0, 10);

    ImGui::Text("Angle");
    ImGui::SliderInt("##sliderangle", &mAngle, 0, 180);

    ImGui::Text("Axiom");
    ImGui::InputText("##axiominput", mAxiomBuffer, BUFSIZE);

    ImGui::Text("Rules");

    for (size_t i = 0; i < NUM_RULES; i++) {
        char buf[4];
        std::snprintf(buf, sizeof(buf), "##%zu", i);
        ImGui::InputText(buf, mRuleBuffers[i], BUFSIZE);
    }

    if (ImGui::Button("Update")) {
        mLsystem.clear();
        mLsystem.setAxiom({mAxiomBuffer});
        bool new_error = false;
        for (size_t i = 0; i < NUM_RULES; i++) {
            std::string new_rule_str{mRuleBuffers[i]};
            std::pair<char, std::string> new_rule;

            if (new_rule_str.size() == 0) continue;

            if (new_rule_str.size() < 3) {
                new_error = true;
                error_msg = "rule must be at least 3 chars long";
                continue;
            }

            if (!std::isalnum(new_rule_str.at(0))) {
                new_error = true;
                error_msg = "first char in rule must be a letter";
                continue;
            } else {
                new_rule.first = new_rule_str.at(0);
            }

            if (new_rule_str.at(1) != '=') {
                new_error = true;
                error_msg = "second char in rule must be a '='";
                continue;
            }

            new_rule.second = new_rule_str.substr(2);
            mLsystem.addRule(new_rule);
        }
        if (!new_error)
            error = false;
        else
            error = true;
        ImGui::End();
        update();
        sf::FloatRect bounds = mVertices.getBounds();
        view.setCenter(bounds.left + bounds.width / 2, bounds.top + bounds.height / 2);
        return;
    }

    ImGui::Text("%s",
                std::string(std::to_string(mVertices.getVertexCount()) + " Vertices").c_str());

    if (error) ImGui::TextColored({1.f, 0, 0, 1.f}, "%s", error_msg.c_str());

    static int item_current = 0;
    ImGui::Combo("##combo", &item_current, DEMO_NAMES, IM_ARRAYSIZE(DEMO_NAMES));
    ImGui::SameLine();
    if (ImGui::Button("Load")) {
        mLsystem = DEMO_SYSTEMS[item_current];
        mAngle = mLsystem.getAngle();
        updateBuffers();
        update();
    }
    ImGui::End();
}

void LSystemRenderer::draw(sf::RenderWindow& win, sf::View& view)
{
    configWindow(view);
    sf::VertexArray vtest;
    vtest.append({{0, 0}, sf::Color::Red});
    vtest.append({{10, 0}, sf::Color::Red});
    win.draw(vtest);
    // std::printf("%u %f\n", color_palette[0].b, mVertices[1].position.y);

    win.draw(mVertices);
}

void LSystemRenderer::executeLSystemStack(const std::string& lsys, float angle_step,
                                          float dist_step)
{
    mVertices.clear();
    TurtleStatus currentStatus;
    std::stack<TurtleStatus> stack;

    for (char c : lsys) {
        switch (c) {
        case 'F':
            mVertices.append({currentStatus.pos, sf::Color::Green});
            currentStatus.pos.x += std::cos((currentStatus.angle / 360.f) * 2 * PI) * dist_step;
            currentStatus.pos.y += std::sin((currentStatus.angle / 360.f) * 2 * PI) * dist_step;
            mVertices.append({{currentStatus.pos}, sf::Color::Green});
            break;
        case 'G':
            mVertices.append({{currentStatus.pos}, sf::Color::Green});
            currentStatus.pos.x += std::cos((currentStatus.angle / 360.f) * 2 * PI) * dist_step;
            currentStatus.pos.y += std::sin((currentStatus.angle / 360.f) * 2 * PI) * dist_step;
            mVertices.append({{currentStatus.pos}, sf::Color::Green});
            break;
        case '+':
            currentStatus.angle -= angle_step;
            break;
        case '-':
            currentStatus.angle += angle_step;
            break;
        case '>':
            currentStatus.color++;
            currentStatus.color %= IM_ARRAYSIZE(color_palette);
            break;
        case '[':
            stack.push(currentStatus);
            break;
        case ']':
            currentStatus = stack.top();
            stack.pop();
            break;
        }
    }
}

void LSystemRenderer::executeLSystemBranch(const std::string& lsys, size_t& index, sf::Vector2f pos,
                                           float angle, float angle_step, float dist_step,
                                           size_t color)
{
    while (index < lsys.size()) {
        char c = lsys.at(index++);
        switch (c) {
        case 'F':
            mVertices.append({{pos}, color_palette[color]});
            pos.x += std::cos((angle / 360.f) * 2 * PI) * dist_step;
            pos.y += std::sin((angle / 360.f) * 2 * PI) * dist_step;
            mVertices.append({{pos}, color_palette[color]});
            break;
        case 'G':
            // mVertices.append({ {pos}, color_palette[color] });
            pos.x += std::cos((angle / 360.f) * 2 * PI) * dist_step;
            pos.y += std::sin((angle / 360.f) * 2 * PI) * dist_step;
            // mVertices.append({ {pos}, color_palette[color] });
            break;
        case '+':
            angle -= angle_step;
            break;
        case '-':
            angle += angle_step;
            break;
        case '>':
            color++;
            color %= IM_ARRAYSIZE(color_palette);
            break;
        case '[':
            executeLSystemBranch(lsys, index, pos, angle, angle_step, dist_step, color);
            break;
        case ']':
            return;
        }
    }
}

void LSystemRenderer::execLSystem(const std::string& lsys, float angle_step, float dist_step)
{
    mVertices.clear();
    size_t idx = 0;
    size_t col = 0;
    executeLSystemBranch(lsys, idx, {0, 0}, 0, angle_step, dist_step, col);
}
