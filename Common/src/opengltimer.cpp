/*
 *
 * Copyright © 2010-2016 Balázs Tóth <tbalazs@iit.bme.hu>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 */

#include "opengltimer.hpp"

OpenGLTimer::OpenGLTimer()
{
    glGenQueries(2, queryID);
}

OpenGLTimer::~OpenGLTimer()
{
  glDeleteQueries(2, queryID);
}

void OpenGLTimer::start()
{
    glQueryCounter(queryID[0], GL_TIMESTAMP);
}

double OpenGLTimer::stop()
{
    GLuint64 startTime, stopTime;
    glQueryCounter(queryID[1], GL_TIMESTAMP);

    // wait until the results are available
    int stopTimerAvailable = 0;
    while (!stopTimerAvailable) {
        glGetQueryObjectiv(queryID[1],GL_QUERY_RESULT_AVAILABLE,&stopTimerAvailable);
    }

    // get query results
    glGetQueryObjectui64v(queryID[0], GL_QUERY_RESULT, &startTime);
    glGetQueryObjectui64v(queryID[1], GL_QUERY_RESULT, &stopTime);

    return (stopTime - startTime) / 1000000.0;
}
