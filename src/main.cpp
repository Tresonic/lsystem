#include <iostream>
#include <vector>
#include <array>
#include <string>
#include <stack>
#include <chrono>
#include <cmath>

#include <SFML/Graphics.hpp>
#include <imgui.h>
#include <imgui-SFML.h>

//#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
static const sf::Color color_palette[] = { sf::Color::Cyan, sf::Color::Blue, sf::Color::Green, sf::Color::Magenta, sf::Color::Red, sf::Color::Yellow };
constexpr float PI = 3.1415926535f;
//constexpr uint32_t color_palette[] = {0xff0000ff, 0x00ff00ff, 0x0000ffff};

class LSystem {
public:
	LSystem(const std::string& axiom, const std::map<char, std::string>& rules, float angle)
		: mAxiom(axiom), mRules(rules), mAngle(angle)
	{}

	std::string generate(int generation) {
		std::string gen_in;
		std::string gen_out = mAxiom;
		for (int i = 0; i < generation; i++) {
			gen_in = gen_out;
			gen_out.clear();
			gen_out.reserve(gen_in.size() * 2);
			for (char c : gen_in) {
				if (mRules.count(c)) {
					gen_out.append(mRules.at(c));
				}
				else {
					gen_out += c;
				}
			}

		}
		return gen_out;
	}

	void clear() {
		mAxiom.clear();
		mRules.clear();
		mAngle = 0;
	}

	void setAxiom(const std::string& axiom) {
		mAxiom = axiom;
	}

	void addRule(const std::pair<char, std::string>& rule) {
		mRules.insert(rule);
	}

	float getAngle() {
		return mAngle;
	}

	std::string& getAxiom() {
		return mAxiom;
	}

	const std::map<char, std::string>& getRules() {
		return mRules;
	}

private:
	std::string mAxiom;
	std::map<char, std::string> mRules;
	float mAngle;
};


//void executeLSystemBranch(const std::string& lsys, size_t& index, std::vector<sf::VertexArray>& output, sf::Vector2f pos, float angle, float angle_step, float dist_step, sf::Color& color) {
//	sf::VertexArray shape(sf::LineStrip);
//	shape.clear();
//	shape.append({ {pos}, color });
//
//	while(index < lsys.size()) {
//		char c = lsys.at(index++);
//		switch (c) {
//		case 'F':
//			pos.x += std::cos((angle / 360.f) * 2 * 3.14159f) * dist_step;
//			pos.y += std::sin((angle / 360.f) * 2 * 3.14159f) * dist_step;
//			shape.append({ {pos}, color });
//			break;
//		case 'H':
//			pos.x += std::cos((angle / 360.f) * 2 * 3.14159f) * (dist_step / 2);
//			pos.y += std::sin((angle / 360.f) * 2 * 3.14159f) * (dist_step / 2);
//			shape.append({ {pos}, color });
//			break;
//		case 'G':
//			pos.x += std::cos((angle / 360.f) * 2 * 3.14159f) * dist_step;
//			pos.y += std::sin((angle / 360.f) * 2 * 3.14159f) * dist_step;
//			break;
//		case '+':
//			angle -= angle_step;
//			break;
//		case '-':
//			angle += angle_step;
//			break;
//		case '[':
//			executeLSystemBranch(lsys, index, output, pos, angle, angle_step, dist_step, color);
//			break;
//		case ']':
//			output.push_back(shape);
//			return;
//		}
//	}
//	output.push_back(shape);
//}
//
//std::vector<sf::VertexArray> execLSystem(const std::string& lsys, float angle_step, float dist_step) {
//	std::vector<sf::VertexArray> vec;
//	size_t idx = 0;
//	sf::Color col = sf::Color::Red;
//	std::cout << "execLSystem: " << lsys << '\n';
//	executeLSystemBranch(lsys, idx, vec, {0, 0}, 0, angle_step, dist_step, col);
//	return vec;
//}

const char* DEMO_NAMES[] = {
	"snowflake",
	"weed",
	"koch_a",
	"sierpinski_triangle",
	"sierpinski_carpet",
	"wispy_tree",
	"joined_cross_curves",
	"pond_weed"
};
const LSystem DEMO_SYSTEMS[] = {
	{{ "F++F++F" }, { {'F', {"F-F++F-F"}} }, 60},
	{{ "X" }, { {'X', {"F-[[X]+X]+F[+FX]-X"}}, {'F', {"FF"}} }, 25},
	{{ "F-F-F-F" }, { {'F', {"FF-F-F-F-F-F+F"}} }, 90},
	{{ "F-G-G" }, { {'F', {"F-G+F+G-F"}}, {'G', {"GG"}} }, 120},
	{{ "F" }, { {'F', {"F+F-F-F-G+F+F+F-F"}}, {'G', {"GGG"}} }, 90},
	{{ "FX" }, { {'F', {"FF-[-F+F]+[+F-F]"}}, {'X', {"FF+[+F]+[-F]"}} }, 25},
	{{ "XYXYXYX+XYXYXYX+XYXYXYX+XYXYXYX" }, { {'X', {"X=FX+FX+FXFY-FY-"}}, {'Y', {"+FX+FXFY-FY-FY"}} }, 90},
	{{ "F" }, { {'F', {"F=FF[-F++F][+F--F]++F--F"}}}, 27},
};

struct TurtleStatus {
	TurtleStatus()
		: pos{0, 0}, angle{0}, color{0}
	{}

	TurtleStatus(const TurtleStatus& other) 
		: pos{other.pos}, angle{other.angle}, color{other.color}
	{}

	sf::Vector2f pos;
	float angle;
	size_t color;
};

class LSystemRenderer {
public:
	LSystemRenderer()
		:mLsystem({ "F++F++F" }, { {'F', {"F-F++F-F"}} }, 60), mVertices(sf::Lines), mGeneration(5), mAngle(60)
	{
		updateBuffers();
		update();
	}

	void updateBuffers() {
		strcpy(mAxiomBuffer, mLsystem.getAxiom().c_str());
		int i = 0;
		for (auto& rule : mLsystem.getRules()) {
			std::string rule_str = std::string(1, rule.first) + "=" + rule.second;
			strcpy(mRuleBuffers[i++], rule_str.c_str());
		}
	}

	void update() {
		auto t1 = std::chrono::high_resolution_clock::now();
		std::string lsys = mLsystem.generate(mGeneration);
		auto t2 = std::chrono::high_resolution_clock::now();
		//auto ms_int = duration_cast<std::chrono::milliseconds>(t2 - t1);
		//std::cout << "lsys time: " << ms_int << '\n';

		t1 = std::chrono::high_resolution_clock::now();
		execLSystem(lsys, mAngle, 10);
		t2 = std::chrono::high_resolution_clock::now();
		//ms_int = duration_cast<std::chrono::milliseconds>(t2 - t1);
		//std::cout << "recursive: " << ms_int << '\n';

		t1 = std::chrono::high_resolution_clock::now();
		executeLSystemStack(lsys, mAngle, 10);
		t2 = std::chrono::high_resolution_clock::now();
		//ms_int = duration_cast<std::chrono::milliseconds>(t2 - t1);
		//std::cout << "stack: " << ms_int << '\n';

		//t1 = std::chrono::high_resolution_clock::now();
		//executeLSystemStack(lsys, mAngle, 10);
		//t2 = std::chrono::high_resolution_clock::now();
		//ms_int = duration_cast<std::chrono::milliseconds>(t2 - t1);
		//std::cout << "stack: " << ms_int << '\n';
	}

	void configWindow(sf::View& view) {
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
			std::snprintf(buf, sizeof(buf), "##%i", i);
			ImGui::InputText(buf, mRuleBuffers[i], BUFSIZE);
		}

		if (ImGui::Button("Update")) {
			mLsystem.clear();
			mLsystem.setAxiom({ mAxiomBuffer });
			bool new_error = false;
			for (size_t i = 0; i < NUM_RULES; i++) {
				std::string new_rule_str{ mRuleBuffers[i] };
				std::pair<char, std::string> new_rule;

				if (new_rule_str.size() == 0)
					continue;

				if (new_rule_str.size() < 3) {
					new_error = true;
					error_msg = "rule must be at least 3 chars long";
					continue;
				}

				if (!std::isalnum(new_rule_str.at(0))) {
					new_error = true;
					error_msg = "first char in rule must be a letter";
					continue;
				}
				else {
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

		ImGui::Text(std::string(std::to_string(mVertices.getVertexCount()) + " Vertices").c_str());

		if (error)
			ImGui::TextColored({ 1.f, 0, 0, 1.f }, error_msg.c_str());

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

	void draw(sf::RenderWindow& win, sf::View& view) {
		configWindow(view);
		sf::VertexArray vtest;
		vtest.append({ {0, 0}, sf::Color::Red });
		vtest.append({ {10, 0}, sf::Color::Red });
		win.draw(vtest);
		std::printf("%u %f\n", color_palette[0].b, mVertices[1].position.y);


		win.draw(mVertices);
	}

private:
	void executeLSystemStack(const std::string& lsys, float angle_step, float dist_step) {
		mVertices.clear();
		TurtleStatus currentStatus;
		std::stack<TurtleStatus> stack;

		for(char c : lsys) {
			switch (c) {
			case 'F':
				mVertices.append({ currentStatus.pos, sf::Color::Green });
				currentStatus.pos.x += std::cos((currentStatus.angle / 360.f) * 2 * PI) * dist_step;
				currentStatus.pos.y += std::sin((currentStatus.angle / 360.f) * 2 * PI) * dist_step;
				mVertices.append({ {currentStatus.pos}, sf::Color::Green });
				break;
			case 'G':
				mVertices.append({ {currentStatus.pos}, sf::Color::Green });
				currentStatus.pos.x += std::cos((currentStatus.angle / 360.f) * 2 * PI) * dist_step;
				currentStatus.pos.y += std::sin((currentStatus.angle / 360.f) * 2 * PI) * dist_step;
				mVertices.append({ {currentStatus.pos}, sf::Color::Green });
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

	void executeLSystemBranch(const std::string& lsys, size_t& index, sf::Vector2f pos, float angle, float angle_step, float dist_step, size_t color) {
		while (index < lsys.size()) {
			char c = lsys.at(index++);
			switch (c) {
			case 'F':
				mVertices.append({ {pos}, color_palette[color] });
				pos.x += std::cos((angle / 360.f) * 2 * PI) * dist_step;
				pos.y += std::sin((angle / 360.f) * 2 * PI) * dist_step;
				mVertices.append({ {pos}, color_palette[color] });
				break;
			case 'G':
				//mVertices.append({ {pos}, color_palette[color] });
				pos.x += std::cos((angle / 360.f) * 2 * PI) * dist_step;
				pos.y += std::sin((angle / 360.f) * 2 * PI) * dist_step;
				//mVertices.append({ {pos}, color_palette[color] });
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
	void execLSystem(const std::string& lsys, float angle_step, float dist_step) {
		mVertices.clear();
		size_t idx = 0;
		size_t col = 0;
		executeLSystemBranch(lsys, idx, { 0, 0 }, 0, angle_step, dist_step, col);
	}

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

sf::Vector2f getMouseWorld(sf::RenderWindow& win) {
	return win.mapPixelToCoords(sf::Mouse::getPosition(win));
}

void zoomViewAt(sf::Vector2i pixel, sf::View& view, sf::RenderWindow& window, float zoom)
{
	const sf::Vector2f beforeCoord{ window.mapPixelToCoords(pixel) };
	view.zoom(zoom);
	window.setView(view);
	const sf::Vector2f afterCoord{ window.mapPixelToCoords(pixel) };
	const sf::Vector2f offsetCoords{ beforeCoord - afterCoord };
	view.move(offsetCoords);
}


int main() {
	sf::RenderWindow window(sf::VideoMode(1600, 900), "LSystem", sf::Style::Default);
	window.setFramerateLimit(60);

	ImGui::SFML::Init(window);

	sf::View view;
	view.setCenter(0, 0);
	view.setSize(window.getSize().x, window.getSize().y);
	view.rotate(90);

	//sf::FloatRect bounds = lshape.getBounds();
	//view.setCenter(bounds.left + bounds.width / 2, bounds.top + bounds.height / 2);
	//view.setSize(bounds.width, bounds.height);

	LSystemRenderer lsysrenderer{};

	float zoom = 1;
	bool mouse_pressed = false;
	sf::Vector2f mouseOld{ 0, 0 };

	while (window.isOpen()) {
		sf::Event event;
		sf::Clock deltaClock;

		while (window.pollEvent(event)) {
			ImGui::SFML::ProcessEvent(event);
			if (event.type == sf::Event::Closed)
				window.close();

			if (event.type == sf::Event::Resized) {
				view.setSize(event.size.width, event.size.height);
			}

			if (ImGui::GetIO().WantCaptureMouse) {
				mouse_pressed = false;
				continue;
			}

			if (event.type == sf::Event::MouseWheelScrolled) {
				if (event.mouseWheelScroll.delta > 0) {
					zoomViewAt({ event.mouseWheelScroll.x, event.mouseWheelScroll.y }, view, window, 0.9f);
				}
				else {
					zoomViewAt({ event.mouseWheelScroll.x, event.mouseWheelScroll.y }, view, window, 1.1f);
				}
			}

			if (event.type == sf::Event::MouseMoved && mouse_pressed) {
				auto mouseWorld = getMouseWorld(window);
				auto offset = mouseOld - mouseWorld;
				view.move(offset);
				window.setView(view);
				mouseOld = getMouseWorld(window);
			}

			if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
				mouseOld = getMouseWorld(window);
				mouse_pressed = true;
			}

			if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left)
				mouse_pressed = false;
		}

		ImGui::SFML::Update(window, deltaClock.restart());

		//std::printf("%f %f; %f %f\n", view.getCenter().x, view.getCenter().y, view.getSize().x, view.getSize().y);
		window.clear();
		window.setView(view);

		lsysrenderer.draw(window, view);
		ImGui::SFML::Render(window);
		window.display();
	}

	return 0;
}
