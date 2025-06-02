#ifndef __BASE_MESH__
#define __BASE_MESH__

#include "GL/gl3w.h"
#include "glm/glm.hpp"
#include <vector>

namespace M3D_ISICG
{
	struct BaseMesh
	{
		GLuint vao			= 0;
		GLuint vboPositions = 0;
		GLuint vboColors	= 0;
		GLuint ebo			= 0;

		std::vector<glm::vec3> positions;
		std::vector<glm::vec3> colors;
		std::vector<GLuint>	   indices;

		void uploadToGPU()
		{
			// Generate and bind VAO
			glCreateVertexArrays( 1, &vao );

			// Position VBO
			glCreateBuffers( 1, &vboPositions );
			glNamedBufferData( vboPositions, positions.size() * sizeof( glm::vec3 ), positions.data(), GL_STATIC_DRAW );
			glVertexArrayVertexBuffer( vao, 0, vboPositions, 0, sizeof( glm::vec3 ) );
			glEnableVertexArrayAttrib( vao, 0 );
			glVertexArrayAttribFormat( vao, 0, 3, GL_FLOAT, GL_FALSE, 0 );
			glVertexArrayAttribBinding( vao, 0, 0 );

			// Color VBO
			glCreateBuffers( 1, &vboColors );
			glNamedBufferData( vboColors, colors.size() * sizeof( glm::vec3 ), colors.data(), GL_STATIC_DRAW );
			glVertexArrayVertexBuffer( vao, 1, vboColors, 0, sizeof( glm::vec3 ) );
			glEnableVertexArrayAttrib( vao, 1 );
			glVertexArrayAttribFormat( vao, 1, 3, GL_FLOAT, GL_FALSE, 0 );
			glVertexArrayAttribBinding( vao, 1, 1 );

			// EBO
			glCreateBuffers( 1, &ebo );
			glNamedBufferData( ebo, indices.size() * sizeof( GLuint ), indices.data(), GL_STATIC_DRAW );
			glVertexArrayElementBuffer( vao, ebo );
		}

		void draw()
		{
			glBindVertexArray( vao );
			glDrawElements( GL_TRIANGLES, static_cast<GLsizei>( indices.size() ), GL_UNSIGNED_INT, 0 );
			glBindVertexArray( 0 );
		}

		void destroy()
		{
			glDeleteBuffers( 1, &vboPositions );
			glDeleteBuffers( 1, &vboColors );
			glDeleteBuffers( 1, &ebo );
			glDeleteVertexArrays( 1, &vao );
		}
	};
} // namespace M3D_ISICG

#endif 
