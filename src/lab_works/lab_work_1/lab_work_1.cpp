#include "lab_work_1.hpp"
#include "imgui.h"
#include "utils/read_file.hpp"
#include <iostream>

namespace M3D_ISICG
{
	const std::string LabWork1::_shaderFolder = "src/lab_works/lab_work_1/shaders/";
	
	// Ajout d'un attribut pour stocker l'identifiant du programme
	GLuint				   _program;
	std::vector<glm::vec2> pSommets;
	GLuint				   _vbo;
	GLuint				   _vao;


	LabWork1::~LabWork1() {
		// Ajout de la destruction du programme OpenGL dans le destructeur
		glDeleteProgram( _program );
		glDeleteBuffers( 1, &_vbo );
		glDisableVertexArrayAttrib( _vao, 0 );
		glDeleteVertexArrays( 1, &_vao );
	}

	bool LabWork1::init()
	{
		std::cout << "Initializing lab work 1..." << std::endl;

		const std::string vertexShaderStr   = readFile( _shaderFolder + "lw1.vert" );
		const std::string fragmentShaderStr = readFile( _shaderFolder + "lw1.frag" );
		const GLchar *	  vSrc				= vertexShaderStr.c_str();
		const GLchar *	  fSrc				= fragmentShaderStr.c_str();

		// Set the color used by glClear to clear the color buffer (in render()).
		glClearColor( _bgColor.x, _bgColor.y, _bgColor.z, _bgColor.w );

		//Creation de l'objet shader pour le vertex 'vshader'
		GLuint vshader = glCreateShader( GL_VERTEX_SHADER );
		glShaderSource( vshader, 1, &vSrc, NULL );
		glCompileShader( vshader );

		// Creation de l'objet shader pour le fragment 'fshader'
		GLuint fshader = glCreateShader( GL_FRAGMENT_SHADER );
		glShaderSource( fshader, 1, &fSrc, NULL );
		glCompileShader( fshader );

		// Check if compilation is ok.
		GLint compiled;
		glGetShaderiv( vshader, GL_COMPILE_STATUS, &compiled );
		if ( !compiled )
		{
			GLchar log[ 1024 ];
			glGetShaderInfoLog( vshader, sizeof( log ), NULL, log );
			glDeleteShader( vshader );
			glDeleteShader( fshader );
			std ::cerr << " Error compiling vertex shader : " << log << std ::endl;
			return false;
		}

		// Création de notre program
		GLuint _program = glCreateProgram();

		// On attache nos shaders à notre programme
		glAttachShader( _program, vshader );
		glAttachShader( _program, fshader );
		glLinkProgram( _program );

		// Check if link is ok.
		GLint linked;
		glGetProgramiv( _program, GL_LINK_STATUS, &linked );
		if ( !linked )
		{
			GLchar log[ 1024 ];
			glGetProgramInfoLog( _program, sizeof( log ), NULL, log );
			std ::cerr << " Error linking program : " << log << std ::endl;
			return false;
		}

		// Destruction des shader
		glDeleteShader( vshader );
		glDeleteShader( fshader );

		//Initialisation des coordonnées des sommets du triangle
		pSommets.push_back( glm::vec2( -0.5, 0.5 ) );
		pSommets.push_back( glm::vec2( 0.5, 0.5 ) );
		pSommets.push_back( glm::vec2( 0.5, -0.5 ) );
		
		// Creation du vbo et remplissement du VBO avec les sommets
		glCreateBuffers( 1, &_vbo );
		glBindBuffer( GL_ARRAY_BUFFER, _vbo );
		glNamedBufferData( _vbo, pSommets.size() * sizeof( glm::vec2 ), pSommets.data(), GL_STATIC_DRAW );
		
		// Creation du vao 
		glCreateVertexArrays( 1, &_vao );
		glEnableVertexArrayAttrib( _vao, 0 );
		glVertexArrayAttribFormat( _vao, 0, 2, GL_FLOAT, GL_FALSE, 0 );

		// Liaison du VBO et VAO
		glVertexArrayVertexBuffer(this->_vao, 0, _vbo, 0, sizeof(glm::vec2) );


		// utilisation du programme
		glUseProgram( _program );

		std::cout << "Done!" << std::endl;
		return true;
	}

	void LabWork1::animate( const float p_deltaTime ) {}

	void LabWork1::render() {
		// netoyage du framebuffer
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
		// Lier le Vao au programme
		glBindVertexArray(_vao);
		// GL_TRIANGLE en paramètre pour spécifier de dessiner un triangle avec les 3 sommets
		glDrawArrays( GL_TRIANGLES, 0, 3 );
		glBindVertexArray( 0 );
	}

	void LabWork1::handleEvents( const SDL_Event & p_event )
	{}

	void LabWork1::displayUI()
	{
		ImGui::Begin( "Settings lab work 1" );
		ImGui::Text( "No setting available!" );
		ImGui::End();
	}

} // namespace M3D_ISICG
