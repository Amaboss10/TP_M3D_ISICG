#ifndef __SHADER_UTILS__
#define __SHADER_UTILS__

#include "GL/gl3w.h"
#include "utils/read_file.hpp"
#include <iostream>
#include <string>

namespace M3D_ISICG
{
	inline GLuint compileShader( const GLenum type, const std::string & source )
	{
		GLuint		 shader = glCreateShader( type );
		const char * src	= source.c_str();
		glShaderSource( shader, 1, &src, nullptr );
		glCompileShader( shader );

		GLint success;
		glGetShaderiv( shader, GL_COMPILE_STATUS, &success );
		if ( !success )
		{
			GLchar log[ 1024 ];
			glGetShaderInfoLog( shader, 1024, nullptr, log );
			std::cerr << "Error compiling shader: " << log << std::endl;
			glDeleteShader( shader );
			return 0;
		}
		return shader;
	}

	inline GLuint createProgramFromSources( const std::string & vertexSource, const std::string & fragmentSource )
	{
		GLuint vertexShader	  = compileShader( GL_VERTEX_SHADER, vertexSource );
		GLuint fragmentShader = compileShader( GL_FRAGMENT_SHADER, fragmentSource );
		if ( vertexShader == 0 || fragmentShader == 0 )
			return 0;

		GLuint program = glCreateProgram();
		glAttachShader( program, vertexShader );
		glAttachShader( program, fragmentShader );
		glLinkProgram( program );

		GLint success;
		glGetProgramiv( program, GL_LINK_STATUS, &success );
		if ( !success )
		{
			GLchar log[ 1024 ];
			glGetProgramInfoLog( program, 1024, nullptr, log );
			std::cerr << "Error linking program: " << log << std::endl;
			glDeleteProgram( program );
			program = 0;
		}

		glDeleteShader( vertexShader );
		glDeleteShader( fragmentShader );

		return program;
	}

	inline GLuint createProgramFromFiles( const std::string & vertexPath, const std::string & fragmentPath )
	{
		const std::string vertexSource	 = readFile( vertexPath );
		const std::string fragmentSource = readFile( fragmentPath );
		return createProgramFromSources( vertexSource, fragmentSource );
	}

} // namespace M3D_ISICG
#endif
