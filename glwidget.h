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
#ifndef GLWIDGET_H
#define GLWIDGET_H

#define GLEW_STATIC
#include <GL/glew.h>

#include <QGLWidget>
#include <QGLShaderProgram>
#include <QPointF>

class GLWidget : public QGLWidget
{
	Q_OBJECT

public:
	explicit GLWidget(QWidget *parent = 0);
	~GLWidget();

protected:
	void initializeGL();
	void paintGL();
	void resizeGL(int w, int h);

	void keyPressEvent(QKeyEvent *);
	void wheelEvent(QWheelEvent *);

	void renderSnake();

private:
	QGLShaderProgram m_program;	
	float m_zoom;

	GLuint m_grassTexture;
	QPointF m_snakePos;
	QMap<QString, GLuint> m_snakeTextures;
	GLuint m_currentSnakeTexture;

	friend class MainWindow;
};

#endif // GLWIDGET_H

