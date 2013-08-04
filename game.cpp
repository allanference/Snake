/*
 * Copyright (c) 2013 Ahmed Samy  <f.fallen45@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#include "game.h"
#include "shadersources.h"

#include <iostream>
#include <sstream>
#include <cmath>
#include <cstdlib>
#include <assert.h>

Game::Game() :
	  m_width(DEFAULT_WIDTH),
	  m_height(DEFAULT_HEIGHT),
	  m_lastInterval(400),
	  m_applesEaten(0),
	  m_zoom(1.0f),
	  m_newApple(true),
	  m_snake(nullptr),
	  m_appleTile(nullptr)
{
}

Game::~Game()
{
	m_map.clear();
}

void Game::createMapTiles()
{
	TilePtr newTile;

	int x, y;
	for (x = 0, y = 0 ;; x += 32) {
		if (x >= m_width) {
			m_viewportWidth = x - 32;
			x = 0;
			y += 32;
		}
		// Make sure we don't render offscreen
		if (y >= m_height)
			break;

		newTile = TilePtr(new Tile(Point(x, y)));
		newTile->addTexture(m_grassTexture);
		m_map.addTile(newTile);
	}

	m_viewportHeight = y - 32;
}

void Game::makeApple()
{
	if (!m_newApple)
		return;

	int randomIndex = rand() % 8;
	const TexturePtr& foodTex = m_appleTextures[randomIndex];
	if (!foodTex) {
		std::cerr << "Internal error: No apple texture at index " << randomIndex << ", aborting..." << std::endl;
		std::abort();
	}

	/* Figure out place position.  */
	TilePtr snakeTile = m_snake->tile();
	TilePtr placeTile = nullptr;
	do
		placeTile = m_map.getRandomTile();
	while (placeTile && placeTile->pos() == snakeTile->pos());

	if (!placeTile) {
		/* Impossible to reach here...  */
		std::cerr << "Internal error: Could find a suitable tile to place the food over, aborting..." << std::endl;
		std::abort();
	}

	m_appleTile = placeTile;
	m_appleTile->addTexture(foodTex);
	m_newApple = false;
}

bool Game::initialize()
{
	m_program.create();
	if (!m_program.compile(GL_VERTEX_SHADER, vertexSource))
		return false;

	if (!m_program.compile(GL_FRAGMENT_SHADER, fragmentSource))
		return false;

	m_program.bindAttribLocation(Position, "vertex");
	m_program.bindAttribLocation(TexCoord, "texcoord");
	if (!m_program.link()) {
		std::cerr << "Failed to link the GL shader program: " << m_program.log() << std::endl;
		return false;
	}
	m_program.bind();

	m_grassTexture = TexturePtr(new Texture);
	if (!m_grassTexture->loadTexture("textures/grass.png")) {
		std::cerr << "Failed to load the grass texture." << std::endl;
		return false;
	}

	for (int i = 0; directions[i]; ++i) {
		std::stringstream ss;
		ss << "textures/snake_" << directions[i] << ".png";

		TexturePtr newTexture(new Texture);
		if (!newTexture->loadTexture(ss.str())) {
			std::cerr << "Failed to load Snake Texture from: " << ss.str() << std::endl;
			continue;
		}

		m_snakeTextures[i] = newTexture;
	}
	m_snake = new Snake;

	for (int i = 1; i < 9; i++) {
		std::stringstream ss;
		ss << "textures/food/apple" << i << ".png";

		TexturePtr newTexture(new Texture);
		if (!newTexture->loadTexture(ss.str())) {
			std::cerr << "Failed to load Snake Texture from: " << ss.str() << std::endl;
			continue;
		}
		m_appleTextures[i - 1] = newTexture;
	}

	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	return true;
}

void Game::render()
{
	glClear(GL_COLOR_BUFFER_BIT);

	for (const TilePtr& tile : m_map.getTiles())
		for (const TexturePtr& texture : tile->getTextures())
			renderAt(tile->pos(), texture);
	if (m_newApple)
		makeApple();
}

void Game::resize(int w, int h)
{
	m_width  = w;
	m_height = h;

	glViewport(0, 0, w, h);
	updateProjectionMatrix();

	m_map.clear();
	createMapTiles();

	static bool firstTime = true;
	if (firstTime) {
		m_snake->setTile(m_map.getTile(Point(32, 32)));
		m_snake->setTexture(m_snakeTextures[0]);
		m_snake->setDirection(DIRECTION_RIGHT);
		firstTime = false;
	}

	if (m_appleTile) {
		Point applePos = m_appleTile->pos();
		// If we are resized from a high size into
		// a low one, then we need to reposition the
		// apple to fit the scene viewport.
		// However, instead of doing such job,
		// we will just create a new apple elsewhere.
		if (applePos.x() >= w || applePos.y() >= h) {
			TilePtr snakeTile = m_snake->tile();
			TilePtr placeTile = nullptr;
			do
				placeTile = m_map.getRandomTile();
			while (placeTile && placeTile->pos() == snakeTile->pos());

			if (!placeTile) {
				/* Impossible to reach here...  */
				std::cerr << "Internal error: Could find a suitable tile to place the food over, aborting..." << std::endl;
				std::abort();
			}

			const TexturePtr& appleTexture = m_appleTile->getTextures()[1];
			m_appleTile = placeTile;
			m_appleTile->addTexture(appleTexture);
		} else {
			// Remove the tile at the last apple position
			m_map.removeTile(applePos);
			// Now render the apple at it's previous position.
			m_map.addTile(m_appleTile);
		}
	}
}

void Game::updateProjectionMatrix()
{
	float w = m_width;
	float h = m_height;
	//  Coordinate      Projection Matrix			        GL Coordinate (Transformed)
	//                  | 2.0 / width |   0.0           | 0.0  |
	//  | x  y  1 |  *  | 0.0         |  -2.0 / height  | 0.0  | =  | x' y' 1 |
	//                  |-1.0         |   1.0           | 1.0  |
	GLfloat projectionMatrix[] = {
		 2.0f/w*m_zoom,	  0.0f,		 0.0f,
		 0.0f,		  2.0f/h*m_zoom, 0.0f,
		-1.0f,		 -1.0f,		 1.0f
	};

	m_program.setProjectionMatrix(projectionMatrix);
}

void Game::updateZoom(float zoom)
{
	m_zoom += zoom;
	updateProjectionMatrix();
}

void Game::setSnakeDirection(Direction_t dir)
{
	m_snake->setDirection(dir);

	TexturePtr newTexture;
	switch (dir) {
	case DIRECTION_UP:
		newTexture = m_snakeTextures[3];
		break;
	case DIRECTION_DOWN:
		newTexture = m_snakeTextures[2];
		break;
	case DIRECTION_RIGHT:
		newTexture = m_snakeTextures[0];
		break;
	case DIRECTION_LEFT:
		newTexture = m_snakeTextures[1];
		break;
	case DIRECTION_INVALID:
	default:
		std::cerr << "Invalid direction!" << std::endl;
		return;
	}
	m_snake->setTexture(newTexture);
}

void Game::updateSnakePos()
{
	Point movePos = m_snake->move(m_viewportWidth, m_viewportHeight);
	TilePtr moveTile = m_map.getTile(movePos);
	if (!moveTile) {
		std::cerr << "Internal error: Failed to find a tile to move the snake on."
			<< " Move pos: " << movePos << std::endl;
		return;
	}

	if (movePos == m_appleTile->pos()) {
		++m_applesEaten;
		m_newApple = true;
		m_appleTile->removeTexture(m_appleTile->getTextures()[1]);
		if (m_lastInterval - (m_applesEaten * 3) >= 200)
			m_lastInterval -= m_applesEaten * 3;
	}

	moveTile->addTexture(m_snake->tile()->popTexture());
	m_snake->setTile(moveTile);
}

void Game::renderAt(const Point& pos, const TexturePtr& texture)
{
	static GLubyte indices[] = {
		0, 1, 2,
		0, 2, 3
	};

	static GLfloat texcoord[] = {
		0, 0,
		1, 0,
		1, 1,
		0, 1
	};

	m_program.setVertexData(Position, m_map.transform2D(pos).data(), 2);
	m_program.setVertexData(TexCoord, texcoord, 2);

	texture->bind();
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, indices);
}

