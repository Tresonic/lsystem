#include "LSystem.h"

std::string LSystem::generate(int generation)
{
    std::string gen_in;
    std::string gen_out = mAxiom;
    for (int i = 0; i < generation; i++) {
        gen_in = gen_out;
        gen_out.clear();
        gen_out.reserve(gen_in.size() * 2);
        for (char c : gen_in) {
            if (mRules.count(c)) {
                gen_out.append(mRules.at(c));
            } else {
                gen_out += c;
            }
        }
    }
    return gen_out;
}

void LSystem::clear()
{
    mAxiom.clear();
    mRules.clear();
    mAngle = 0;
}
