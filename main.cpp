#include <SFML/Graphics.hpp>
#include <Eigen>
#include <iostream>
#include <cstdlib>
#include <vector>
#include <chrono>
#include "math.h"

class Giblet {
public:
    float f_size;
    float f_value;
    float f_color;
    float f_xposition;
    float f_yposition;
    void set_values(int, int, float, float);
    sf::CircleShape shape;
};

void Giblet::set_values(int max, int min, float pf_xposition, float pf_yposition) {
    f_size = (min + rand() % (( max + 1 ) - min)) / 1000;
    f_value = M_PI * f_size * f_size;
    shape.setRadius(f_size);
    f_xposition = pf_xposition;
    f_yposition = pf_yposition;
	shape.setPosition(f_xposition, f_yposition);
}

class Organism {
public:
    float f_size;
    float f_xposition;
    float f_yposition;
    float f_rotation = 10*M_PI/180;
    float f_sight_range = 100;
    sf::CircleShape shape;
	void set_values(float, float, float);
	void update_values();
    sf::Vertex sight_line[2];

	std::vector<int> v3i_Color = {0, 0, 0};   
};

void Organism::set_values(float pf_size, float pf_xposition, float pf_yposition) {
    f_size = pf_size;
    shape.setRadius(f_size);
    f_xposition = pf_xposition;
    f_yposition = pf_yposition;
	shape.setPosition(f_xposition, f_yposition);
	sight_line[0] = sf::Vector2f(f_xposition, f_yposition);
	sight_line[1] = sf::Vector2f(f_xposition + (f_sight_range * sin(f_rotation)), f_yposition + (f_sight_range * cos(f_rotation)));
	//sight_line[1] = sf::Vector2f(f_xposition + 20, f_yposition + 20);
}

void Organism::update_values() {
	sight_line[0] = sf::Vector2f(f_xposition, f_yposition);
	sight_line[1] = sf::Vector2f(f_xposition + (f_sight_range * sin(f_rotation)), f_yposition + (f_sight_range * cos(f_rotation)));
}

int main()
{


    //Settings
    int i_organism_count = 10;
    int i_framerate = 60;
    int i_world_size = 1600;
    int i_grid_size = 10;
    float f_food_spawn_chance = 1000; //(out of 1000000000)

    //These will be divided by 1000
    int i_max_food_size = 1000; 
    int i_min_food_size = 1000;

    //Initialise variables
    std::vector<Giblet> v_giblet_list;


    //Generate random seed
    srand(time(0));

    //Create List of creatures
    std::vector<Organism> v_organism_list;
    for(int i = 0; i < i_organism_count; i++) {
        Organism new_organism;
        new_organism.set_values(5, rand() % i_world_size, rand() % i_world_size);
        v_organism_list.push_back(new_organism);
    }

    //Set up SFML window
    sf::RenderWindow window(sf::VideoMode(1600, 1600), "Peytri <3");
    sf::CircleShape shape(100.f);
    window.sf::RenderWindow::setFramerateLimit(i_framerate);
    shape.setFillColor(sf::Color::Green);


    //Repeating Event
    while (window.isOpen())
    {

        //Food spawn loop
        if (f_food_spawn_chance > (rand() % 1000000000)) {
            Giblet new_giblet;
            new_giblet.set_values(i_max_food_size, i_min_food_size, rand() % i_world_size, rand() % i_world_size);
            v_giblet_list.push_back(new_giblet);
        }

        //Organism loop



        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                 window.close();
        }

        window.clear();
        for(int i = 0; i < v_organism_list.size(); i++) {
            //v_organism_list[i].shape.sf::CircleShape::setRadius(v_organism_list[i].f_size); 
            //sf::CircleShape shapeToDraw = v_organism_list[i].shape;
            //shapeToDraw.setRadius(v_organism_list[i].f_size);
            //shapeToDraw.setPosition(v_organism_list[i].f_xposition, v_organism_list[i].f_yposition);
            window.draw(v_organism_list[i].shape);
			v_organism_list[i].update_values();
			v_organism_list[i].f_rotation += 2*M_PI/180;
			window.draw(v_organism_list[i].sight_line, 2, sf::LinesStrip);
            //std::cout << v_organism_list[i].f_xposition << ", "  << v_organism_list[i].f_yposition << "\n";
        }
        for(int i = 0; i < v_giblet_list.size(); i++) {
            //v_organism_list[i].shape.sf::CircleShape::setRadius(v_organism_list[i].f_size); 
            //sf::CircleShape shapeToDraw = v_giblet_list[i].shape;
            //shapeToDraw.setRadius(v_organism_list[i].f_size);
            //shapeToDraw.setPosition(v_giblet_list[i].f_xposition, v_giblet_list[i].f_yposition);
            window.draw(v_giblet_list[i].shape);
            //std::cout << v_organism_list[i].f_xposition << ", "  << v_organism_list[i].f_yposition << "\n";
        }
// define a 100x100 square, red, with a 10x10 texture mapped on it

        window.display();
    }

    return 0;
}

//transform.rotate(angle, center);