#pragma once

#include <map>
#include <string>

class LSystem {
public:
    LSystem(const std::string& axiom, const std::map<char, std::string>& rules, float angle) :
        mAxiom(axiom), mRules(rules), mAngle(angle)
    {}

    std::string generate(int generation);

    void clear();

    void setAxiom(const std::string& axiom) { mAxiom = axiom; }

    void addRule(const std::pair<char, std::string>& rule) { mRules.insert(rule); }

    float getAngle() { return mAngle; }

    std::string& getAxiom() { return mAxiom; }

    const std::map<char, std::string>& getRules() { return mRules; }

private:
    std::string mAxiom;
    std::map<char, std::string> mRules;
    float mAngle;
};

static const char* DEMO_NAMES[] = {"snowflake",
                                   "weed",
                                   "koch_a",
                                   "sierpinski_triangle",
                                   "sierpinski_carpet",
                                   "wispy_tree",
                                   "joined_cross_curves",
                                   "pond_weed"};
static const LSystem DEMO_SYSTEMS[] = {
    {{"F++F++F"}, {{'F', {"F-F++F-F"}}}, 60},
    {{"X"}, {{'X', {"F-[[X]+X]+F[+FX]-X"}}, {'F', {"FF"}}}, 25},
    {{"F-F-F-F"}, {{'F', {"FF-F-F-F-F-F+F"}}}, 90},
    {{"F-G-G"}, {{'F', {"F-G+F+G-F"}}, {'G', {"GG"}}}, 120},
    {{"F"}, {{'F', {"F+F-F-F-G+F+F+F-F"}}, {'G', {"GGG"}}}, 90},
    {{"FX"}, {{'F', {"FF-[-F+F]+[+F-F]"}}, {'X', {"FF+[+F]+[-F]"}}}, 25},
    {{"XYXYXYX+XYXYXYX+XYXYXYX+XYXYXYX"},
     {{'X', {"X=FX+FX+FXFY-FY-"}}, {'Y', {"+FX+FXFY-FY-FY"}}},
     90},
    {{"F"}, {{'F', {"F=FF[-F++F][+F--F]++F--F"}}}, 27},
};