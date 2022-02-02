#include <SFML/Graphics.hpp>
#include <Eigen>
#include <iostream>
#include <cstdlib>
#include <vector>
#include <chrono>
#include <cmath>
#include "math.h"

class Giblet {
public:
    float size;
    float value;
    float color;
    float xposition;
    float yposition;
    void set_values(int, int, float, float);
    sf::CircleShape shape;
};

void Giblet::set_values(int max, int min, float pxposition, float pyposition) {
    size = (min + rand() % (( max + 1 ) - min)) / 1000;
    value = M_PI * size * size;
    shape.setRadius(size);
    xposition = pxposition;
    yposition = pyposition;
	shape.setPosition(xposition, yposition);
}

class Organism {
public:
    float size;
    float xposition;
    float yposition;
	float vx;
	float vy;
    float rotation = 10*M_PI/180;
	float sight_cone_range;
    float sight_range = 100;
	sf::Color vision_color = sf::Color::White; 
    sf::CircleShape shape;
	void set_values(float, float, float, float);
	void update_values();
    //sf::Vertex sight_line[2];
	sf::Vertex sight_cone[4];// = {sf::Vector2f(10, 10), sf::Vector2f(100, 0), sf::Vector2f(0, 90)};
	

	std::vector<int> v3Color = {0, 0, 0};   
};

void Organism::set_values(float psize, float pxposition, float pyposition, float psight_cone_range) {
    size = psize;
    shape.setRadius(size);
    xposition = pxposition;
    yposition = pyposition;
	shape.setOrigin(psize, psize);
	shape.setPosition(xposition /* - sqrt(size / 2)*/, yposition /*- sqrt(size / 2)*/);
	sight_cone_range = psight_cone_range * M_PI/180;
	//sight_line[0] = sf::Vector2f(xposition, yposition);
	//sight_line[1] = sf::Vector2f(xposition + (sight_range * sin(rotation)), yposition + (sight_range * cos(rotation)));
	sight_cone[0] = sf::Vector2f(xposition, yposition);
	sight_cone[1] = sf::Vector2f(xposition + (sight_range * sin(rotation - sight_cone_range)), yposition + (sight_range * cos(rotation - sight_cone_range)));
	sight_cone[2] = sf::Vector2f(xposition + (sight_range * sin(rotation + sight_cone_range)), yposition + (sight_range * cos(rotation + sight_cone_range)));
	sight_cone[3] = sf::Vector2f(xposition, yposition);
	//sight_line[1] = sf::Vector2f(xposition + 20, yposition + 20);

}

void Organism::update_values() {
	//sight_line[0] = sf::Vector2f(xposition, yposition);
	//sight_line[1] = sf::Vector2f(xposition + (sight_range * sin(rotation)), yposition + (sight_range * cos(rotation)));
	//sight_cone[0].color = sf::Color::Red;
	//sight_line[1].color = sf::Color::Red;
	sight_cone[0] = sf::Vector2f(xposition, yposition);
	sight_cone[1] = sf::Vector2f(xposition + (sight_range * sin(rotation - sight_cone_range)), yposition + (sight_range * cos(rotation - sight_cone_range)));
	sight_cone[2] = sf::Vector2f(xposition + (sight_range * sin(rotation + sight_cone_range)), yposition + (sight_range * cos(rotation + sight_cone_range)));
	sight_cone[3] = sf::Vector2f(xposition, yposition);
	shape.setPosition(xposition /* - sqrt(size / 2)*/, yposition /*- sqrt(size / 2)*/);
	xposition += vx;
	yposition += vy;
}

template<class T> bool signTheSame(T t1, T t2, T t3)
{
return t1 < 0 == t2 < 0 && t1 < 0 == t3 < 0 && t2 < 0 == t3 < 0;
}

float cross_product_2d(sf::Vertex pa, sf::Vertex pb) {
	sf::Vector2f a = pa.position;
	sf::Vector2f b = pb.position;
	return (a.x * b.y - a.y * b.x);
}

float cross_product(sf::Vertex a1, sf::Vertex a2, sf::Vertex b1, sf::Vertex b2){
	float va1a2[2] = {a2.position.x-a1.position.x, a2.position.y-a1.position.y};
	float vb1b2[2] = {b2.position.x-b1.position.x, b2.position.y-b1.position.y};
	float cpvavb = (va1a2[0] * vb1b2[1]) - (vb1b2[0] * va1a2[1]);
	return cpvavb;
}



int main()
{

	
	
    //Settings
    int organism_count = 100;
    int framerate = 60;
    int world_size = 1600;
    int grid_size = 50;
    float food_spawn_chance = 1000; //(out of 1000000000)
	int physics_check_interval = 2; //in frames
	int vision_check_interval = 6; //in frames
	float speed_decay = 0.01;

    //These will be divided by 1000
    int max_food_size = 1000; 
    int min_food_size = 1000;

    //Initialise variables
    std::vector<Giblet> giblet_list;


    //Generate random seed
    srand(time(0));

    //Create List of creatures
    std::vector<Organism> organism_list;
    for(int i = 0; i < organism_count; i++) {
        Organism new_organism;
        new_organism.set_values(5, rand() % world_size, rand() % world_size, 20);
        organism_list.push_back(new_organism);
    }
	//Organism new_organism;
	//new_organism.set_values(5, 800, 800, 2);
	//organism_list.push_back(new_organism);

    //Set up SFML window
    sf::RenderWindow window(sf::VideoMode(1600, 1600), "Peytri <3");
    //sf::CircleShape shape(100.f);
    window.sf::RenderWindow::setFramerateLimit(framerate);
    //shape.setFillColor(sf::Color::Green);


	//Draw a grid
	std::vector<sf::Vertex> grid_line_list_a;
	std::vector<sf::Vertex> grid_line_list_b;
	for (int i = 0; i < int(world_size / grid_size); i++) {
			//sf::Vertex grid_line_list_entry[2];
			grid_line_list_a.push_back (sf::Vector2f(grid_size * i, 0));
			grid_line_list_b.push_back (sf::Vector2f(grid_size * i, world_size));
			//grid_line_list_a.push_back(grid_line_list_entry);
			
			grid_line_list_a.push_back (sf::Vector2f(0, grid_size * i));
			grid_line_list_b.push_back (sf::Vector2f(world_size, grid_size * i));
	}

	int physics_frame = 0;
	int vision_frame = 0;
    //Repeating Event
    while (window.isOpen())
    {
		//keyboard input 
		organism_list[0].vx = 0;
		organism_list[0].vy = 0;
		float speed = 3;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
		{
			organism_list[0].vx = -speed;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
		{
			organism_list[0].vx = speed;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
		{
			organism_list[0].vy = -speed;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
		{
			organism_list[0].vy = speed;
		}
		
		
		//physics_frame counter
		if (physics_frame >= physics_check_interval) {
			physics_frame = 0;
		} else {
			physics_frame++;
		}
		
		if (vision_frame >= vision_check_interval) {
			vision_frame = 0;
		} else {
			vision_frame++;
		}
        //Food spawn loop
        if (food_spawn_chance > (rand() % 1000000000)) {
            Giblet new_giblet;
            new_giblet.set_values(max_food_size, min_food_size, rand() % world_size, rand() % world_size);
			giblet_list.push_back(new_giblet);
			/*
			Organism new_organism;
			new_organism.set_values(5, 800 + (((rand() % 2) * 2) - 1) * 5, 800 + (((rand() % 2) * 2) - 1) * 5);
			organism_list.push_back(new_organism);
			organism_list[organism_list.size() - 1].vx = 0;
			organism_list[organism_list.size() - 1].vy = 0;
			*/
        }

		//Physics loop
		for(int i = 0; i < organism_list.size(); i++) {
			if (i % physics_check_interval == physics_frame){
				for(int j = 0; j < organism_list.size(); j++) {
					sf::Vector2f collision_vector;
					float sum_radius = pow(organism_list[i].size + organism_list[j].size, 2);
					float distance = pow(organism_list[i].xposition - organism_list[j].xposition, 2) + pow(organism_list[i].yposition - organism_list[j].yposition,2 );
					//float distance_temp = sqrt(pow(organism_list[i].xposition - organism_list[j].xposition, 2) + pow(organism_list[i].yposition - organism_list[j].yposition,2 ));
					if ((i != j) && (distance < sum_radius)) {
						collision_vector = {organism_list[i].xposition - organism_list[j].xposition, organism_list[i].yposition - organism_list[j].yposition};
						sf::Vector2f collision_vector_normalised = {collision_vector.x / sqrt(pow(collision_vector.x, 2) + pow(collision_vector.y, 2)), collision_vector.y / sqrt(pow(collision_vector.x, 2) + pow(collision_vector.y, 2))};
						if (distance == 0) {
							organism_list[i].xposition += (((rand() % 2) * 2) - 1) * 5;
							organism_list[i].yposition += (((rand() % 2) * 2) - 1) * 5;
							
						} else {
							organism_list[i].xposition += collision_vector.x / 4;
							organism_list[i].yposition += collision_vector.y / 4;
							organism_list[j].xposition -= collision_vector.x / 4;
							organism_list[j].yposition -= collision_vector.y / 4;
							
							organism_list[i].vx += fabs(organism_list[j].vx) * collision_vector_normalised.x / 2;
							organism_list[i].vy += fabs(organism_list[j].vy) * collision_vector_normalised.y / 2;
							organism_list[j].vx -= fabs(organism_list[i].vx) * collision_vector_normalised.x / 2;
							organism_list[j].vy -= fabs(organism_list[i].vy) * collision_vector_normalised.y / 2;
							//std::cout << distance << std::endl;
						}
					}
				}
			}
		}
		//std::cout << "rand" << (((rand() % 2) * 2) - 1) * 5 << std::endl;
        
		//Organism loop
		for(int i = 0; i < organism_list.size(); i++) {
			//vision check
			if (i % vision_check_interval == vision_frame){
				bool break_var = false;
				for(int j = 0; j < organism_list.size() && !break_var; j++) {
					
					sf::Vertex point = sf::Vector2f(organism_list[j].xposition, organism_list[j].yposition);//
					//std::cout << organism_list[j].xposition << "|"<<  organism_list[j].yposition <<std::endl;
					float cp1 = cross_product(organism_list[i].sight_cone[0], point, organism_list[i].sight_cone[0], organism_list[i].sight_cone[1]);
					float cp2 = cross_product(organism_list[i].sight_cone[1], point, organism_list[i].sight_cone[1], organism_list[i].sight_cone[2]);
					float cp3 = cross_product(organism_list[i].sight_cone[2], point, organism_list[i].sight_cone[2], organism_list[i].sight_cone[3]);
					if (signTheSame(cp1, cp2, cp3)) {
						if (i != j) {
						organism_list[i].vision_color = sf::Color::Red;
						//std::cout << organism_list[j].xposition << "|" << organism_list[j].yposition << "\n";
						break_var = true;
						}
					} else {
						if (!break_var) {
						organism_list[i].vision_color = sf::Color::White;
						}
					}
				}
			}
		}
		
		//organism_list[rand() % organism_list.size()].vx += 1 * (rand() % 2 - 1);
		//organism_list[rand() % organism_list.size()].vy += 1 * (rand() % 2 - 1);
		
		//organism_list[0].vx = 1;
		//organism_list[0].vy = 1;
		
		//std::cout << organism_list.size() << std::endl;

        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                 window.close();
        }

        window.clear();
		for(int i = 0; i < grid_line_list_a.size(); i++) {
			//window.draw(grid_line_list[0], 2, sf::LinesStrip);
			grid_line_list_a[i].color = sf::Color(100, 100, 100);
			grid_line_list_b[i].color = sf::Color(100, 100, 100);
			sf::Vertex grid_line_temp[2] = {grid_line_list_a[i], grid_line_list_b[i]};		
			//grid_line_temp.Color = sf::Color(100, 100, 100);
			window.draw(grid_line_temp, 2, sf::LinesStrip);	
		}
        for(int i = 0; i < organism_list.size(); i++) {
            //organism_list[i].shape.sf::CircleShape::setRadius(organism_list[i].size); 
            //sf::CircleShape shapeToDraw = organism_list[i].shape;
            //shapeToDraw.setRadius(organism_list[i].size);
            //shapeToDraw.setPosition(organism_list[i].xposition, organism_list[i].yposition);

			if (organism_list[i].vx > 0) {
				organism_list[i].vx -= speed_decay;
			} else if (organism_list[i].vx < 0) {
				organism_list[i].vx += speed_decay;
			}
			if (organism_list[i].vy > 0) {
				organism_list[i].vy -= speed_decay;
			} else if (organism_list[i].vy < 0) {
				organism_list[i].vy += speed_decay;
			}
			organism_list[i].update_values();
			
			//window.draw(organism_list[i].sight_line, 2, sf::LinesStrip);
			window.draw(organism_list[i].shape);
			//if (cross_product_2d(organism_list[0].sight_cone[0], sf::Vector2f(0,0)) < 0) {
			//	organism_list[0].sight_cone[0].color = sf::Color::Red;
			//}
			
			organism_list[i].rotation += 0.01;

			
			//organism_list[0].sight_cone[1].color = sf::Color::Red;
			organism_list[i].sight_cone[0].color = organism_list[i].vision_color;
			organism_list[i].sight_cone[1].color = organism_list[i].vision_color;
			organism_list[i].sight_cone[2].color = organism_list[i].vision_color;
			organism_list[i].sight_cone[3].color = organism_list[i].vision_color;
			
			window.draw(organism_list[i].sight_cone, 4, sf::LinesStrip);
			
            //std::cout << organism_list[i].xposition << ", "  << organism_list[i].yposition << "\n";
        }
			//std::cout << cp1 << std::endl;
			//float cross_product(sf::Vertex[2] va, sf::Vertex[2] vb){
			//	return 0;
			//}
			//std::cout << organism_list[0].sight_cone[0].position.x << ", "<< organism_list[0].sight_cone[0].position.y << " | "  << organism_list[0].sight_cone[0].position.x << ", " << organism_list[0].sight_cone[0].position.y << " | "<< cross_product_2d(organism_list[0].sight_cone[0], sf::Vector2f(1, 1)) << std::endl;
			//float tempx = organism_list[0].sight_cone[0].position.x;
			//float tempy = organism_list[0].sight_cone[0].position.y;
			
			/*std::cout << cross_product_2d(organism_list[0].sight_cone[0], organism_list[0].sight_cone[1]) * cross_product_2d(organism_list[0].sight_cone[1], sf::Vector2f(800, 800)) << "|" <<cross_product_2d(organism_list[0].sight_cone[1], organism_list[0].sight_cone[2]) * cross_product_2d(organism_list[0].sight_cone[2], sf::Vector2f(800, 800)) << "|" << cross_product_2d(organism_list[0].sight_cone[2], organism_list[0].sight_cone[3]) * cross_product_2d(organism_list[0].sight_cone[2], sf::Vector2f(800, 800)) << std::endl;
			if (signTheSame(cross_product_2d(organism_list[0].sight_cone[0], organism_list[0].sight_cone[1]) * cross_product_2d(organism_list[0].sight_cone[1], sf::Vector2f(800, 800)), cross_product_2d(organism_list[0].sight_cone[1], organism_list[0].sight_cone[2]) * cross_product_2d(organism_list[0].sight_cone[2], sf::Vector2f(800, 800)), cross_product_2d(organism_list[0].sight_cone[2], organism_list[0].sight_cone[3]) * cross_product_2d(organism_list[0].sight_cone[2], sf::Vector2f(800, 800)))) {
				organism_list[0].sight_cone[0].color = sf::Color::Red;
				organism_list[0].sight_cone[1].color = sf::Color::Red;
				organism_list[0].sight_cone[2].color = sf::Color::Red;
				organism_list[0].sight_cone[3].color = sf::Color::Red;
				std::cout << "bingo";
				//organism_list[0].sha
			} else {
				organism_list[0].sight_cone[0].color = sf::Color::White;
				organism_list[0].sight_cone[1].color = sf::Color::White;
				organism_list[0].sight_cone[2].color = sf::Color::White;
				organism_list[0].sight_cone[3].color = sf::Color::White;
			}
			*/
			//std::cout <<signTheSame(cross_product_2d(organism_list[0].sight_cone[0], organism_list[0].sight_cone[1]) * cross_product_2d(organism_list[0].sight_cone[1], sf::Vector2f(800, 800)), cross_product_2d(organism_list[0].sight_cone[1], organism_list[0].sight_cone[2]) * cross_product_2d(organism_list[0].sight_cone[2], sf::Vector2f(800, 800)), cross_product_2d(organism_list[0].sight_cone[2], organism_list[0].sight_cone[3]) * cross_product_2d(organism_list[0].sight_cone[2], sf::Vector2f(800, 800))) << std::endl;
			//std::cout << cross_product_2d(organism_list[0].sight_cone[0], sf::Vector2f(1, 1)) * cross_product_2d(organism_list[0].sight_cone[1], sf::Vector2f(1, 1)) << std::endl;
        for(int i = 0; i < giblet_list.size(); i++) {
            //organism_list[i].shape.sf::CircleShape::setRadius(organism_list[i].size); 
            //sf::CircleShape shapeToDraw = giblet_list[i].shape;
            //shapeToDraw.setRadius(organism_list[i].size);
            //shapeToDraw.setPosition(giblet_list[i].xposition, giblet_list[i].yposition);
            window.draw(giblet_list[i].shape);
            //std::cout << organism_list[i].xposition << ", "  << organism_list[i].yposition << "\n";
        }


        window.display();
    }

    return 0;
}

//transform.rotate(angle, center);