#include "imgui.h"
#include "lab_work_2.hpp"
#include "utils/read_file.hpp"
#include <iostream>
#include "glm/gtc/type_ptr.hpp"

namespace M3D_ISICG
{
	const std::string LabWork2::_shaderFolder = "src/lab_works/lab_work_2/shaders/";

	// Ajout d'un attribut pour stocker l'identifiant du programme
	GLuint				   program;
	std::vector<glm::vec2> ptSommets;
	std::vector<glm::vec3> colors;
	GLuint				   vbo;
	GLuint				   colorVBO;
	GLuint				   vao;
	GLuint				   ebo;

	LabWork2::~LabWork2()
	{
		// Ajout de la destruction du programme OpenGL dans le destructeur
		glDeleteProgram( program );
		glDeleteBuffers( 1, &vbo );
		glDeleteBuffers( 1, &colorVBO );
		glDeleteVertexArrays( 1, &vao );
	}

	bool LabWork2::init() 
	{
		std::cout << "Initializing lab work 2..." << std::endl;

		const std::string vertexShaderStr	= readFile( _shaderFolder + "lw2.vert" );
		const std::string fragmentShaderStr = readFile( _shaderFolder + "lw2.frag" );
		const GLchar *	  vSrc				= vertexShaderStr.c_str();
		const GLchar *	  fSrc				= fragmentShaderStr.c_str();

		// Set the color used by glClear to clear the color buffer (in render()).
		glClearColor( _bgColor.x, _bgColor.y, _bgColor.z, _bgColor.w );

		// Creation de l'objet shader pour le vertex 'vshader'
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

		// Cr�ation de notre program
		GLuint program = glCreateProgram();

		// On attache nos shaders � notre programme
		glAttachShader( program, vshader );
		glAttachShader( program, fshader );
		glLinkProgram( program );

		// Check if link is ok.
		GLint linked;
		glGetProgramiv( program, GL_LINK_STATUS, &linked );
		if ( !linked )
		{
			GLchar log[ 1024 ];
			glGetProgramInfoLog( program, sizeof( log ), NULL, log );
			std ::cerr << " Error linking program : " << log << std ::endl;
			return false;
		}

		// Destruction des shader
		glDeleteShader( vshader );
		glDeleteShader( fshader );

		// Initialisation des coordonn�es des sommets du triangle
		ptSommets.push_back( glm::vec2( -0.5, 0.5 ) );
		ptSommets.push_back( glm::vec2( 0.5, 0.5 ) );
		ptSommets.push_back( glm::vec2( 0.5, -0.5 ) );
		ptSommets.push_back( glm::vec2( -0.5, -0.5 ) );
		
		// Indices des sommets formant les 2 triangles du quadrilat�re
		std::vector<int> indices = { 0, 1, 2, 0, 2, 3 };

		//initialisation des couleurs
		colors.push_back(glm::vec3( 1.0f, 0.0f, 0.0f )); //red
		colors.push_back( glm::vec3( 0.0f, 1.0f, 0.0f ) );//green
		colors.push_back( glm::vec3( 0.0f, 0.0f, 1.0f ) );//blue
		colors.push_back( glm::vec3( 1.0f, 0.0f, 1.0f ) ); //magenta

		// nous allons avoir deux uniform afin de faire notre translation du rectangle de gauche a droite, et une pour
		// la luminosite des couleurs
		uTranslationXUniform = glGetUniformLocation( program, "uTranslationX" );
		uLuminosity		   = glGetUniformLocation( program, "uLuminosity" );

		// on envoie a la valeur uLuminosity dans le shader le chiffre 1
		glProgramUniform1f( program, this->uLuminosity, 1 );

		//Ajout de colorVBO pour stocker les couleurs
		glCreateBuffers( 1, &colorVBO );
		glBindBuffer( GL_ARRAY_BUFFER, colorVBO );
		glNamedBufferData( colorVBO, colors.size() * sizeof( glm::vec3), colors.data(), GL_STATIC_DRAW );
		// lier colorVBO au vao
		glCreateVertexArrays( 1, &vao );
		glVertexArrayVertexBuffer( vao, 1, colorVBO, 0, sizeof( glm::vec3 ) );
	
		glEnableVertexArrayAttrib( vao, 1 );
		glVertexArrayAttribFormat( vao, 1, 3, GL_FLOAT, GL_FALSE, 0 );

		// Creation du vbo et remplissement du VBO avec les sommets
		glCreateBuffers( 1, &vbo );
		glBindBuffer( GL_ARRAY_BUFFER, vbo );
		glNamedBufferData( vbo, ptSommets.size() * sizeof( glm::vec2 ), ptSommets.data(), GL_STATIC_DRAW );
		
		// Cr�ation de l'EBO et remplissage avec les indices
		glCreateBuffers( 1, &ebo );
		glNamedBufferData( ebo, indices.size() * sizeof( GLuint ), indices.data(), GL_STATIC_DRAW );
		
		glEnableVertexArrayAttrib( vao, 0 );
		glVertexArrayAttribFormat( vao, 0, 2, GL_FLOAT, GL_FALSE, 0 );

		// Liaison de l'EBO au VAO
		glVertexArrayElementBuffer( vao, ebo );

		// Liaison du VBO et VAO
		glVertexArrayVertexBuffer( this->vao, 0, vbo, 0, sizeof( glm::vec2 ) );

		// utilisation du programme
		glUseProgram( program );

		std::cout << "Done!" << std::endl;
		return true;
	}

	void LabWork2::animate( const float p_deltaTime ) {
		// on va effectuer un translation en fonction du temps et l'envoyer a notre shader
		float translation = glm::sin( this->_time ) / 2;
		glUniform1f( uTranslationXUniform, translation );
		this->_time += p_deltaTime;
	}

	void LabWork2::render()
	{
		// netoyage du framebuffer
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
		// Lier le Vao au programme
		glBindVertexArray( vao );
		// GL_TRIANGLE en param�tre pour sp�cifier de dessiner un triangle avec les 3 sommets
		glDrawElements( GL_TRIANGLES, 6,  GL_UNSIGNED_INT, 0 );
		glBindVertexArray( 0 );
	}

	void LabWork2::handleEvents( const SDL_Event & p_event ) {}

	void LabWork2::displayUI()
	{
		ImGui::Begin( "Settings lab work 2" );
		ImGui::Text( "No setting available!" );

		if ( ImGui::SliderFloat( "Luminosity", &luminosity, 0, 1 ) )
		{
			glProgramUniform1f( program, uLuminosity, luminosity );
		}

		if ( ImGui::ColorEdit3( "color", glm::value_ptr( this->_bgColor ) ) )
			glClearColor( _bgColor.x, _bgColor.y, _bgColor.z, _bgColor.w );


		ImGui::End();
	}

} // namespace M3D_ISICG
