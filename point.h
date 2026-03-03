#pragma once

#include "utils.h"
#include "Directions.h"
#include <cmath>

class Screen; // Forward declaration so point can mention Screen later if needed

class point
{
public:
	int x, y;                                      // Current position on the screen
	char c;                                        // Character used to draw this point
	Direction dir = Direction::directions[Direction::RIGHT]; // Current movement direction (default RIGHT)

public:
	point() {}                                     // Default constructor: does nothing special
	point(int x_1, int y_1, const Direction& the_dir, char ch) {
		x = x_1;                                   // Set x position
		y = y_1;                                   // Set y position
		c = ch;                                   // Set drawing character
		dir = the_dir;                            // Set initial direction
	}
	void draw() {
		draw(c);                                  // Draw using the stored character c
	}
	void draw(char ch);                           // Draw this point with a given character
	void move();                                  // Move point according to current direction
	void changeDir(const Direction& new_dir) {
		dir = new_dir;                            // Change movement direction
	}
	int getX() const {
		return x;                                 // Return x coordinate
	}
	int getY() const {
		return y;                                 // Return y coordinate
	}
	Direction getDir() const {
		return dir;                               // Return current direction
	}
};


