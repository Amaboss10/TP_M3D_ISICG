#include "lab_work_6.hpp"
#include "define.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "imgui.h"

namespace M3D_ISICG
{
	bool LabWork6::init()
	{
		_initCamera();
		_initGBuffer();
		_initGeometryPassProgram();
		_initShadingPassProgram();
		_model.load( "bunny", "data/models/bunny/bunny.obj" );
		_initFullScreenQuad();
		return true;
	}

	void LabWork6::render()
	{
		_geometryPass();
		_shadingPass();
	}

	void LabWork6::animate( const float ) {}

	void LabWork6::_initGBuffer()
	{
		glCreateFramebuffers( 1, &_gBufferFBO );
		glBindFramebuffer( GL_FRAMEBUFFER, _gBufferFBO );
		glGenTextures( 6, _gBufferTextures );
		GLenum attachments[ 5 ];
		for ( int i = 0; i < 5; ++i )
		{
			glBindTexture( GL_TEXTURE_2D, _gBufferTextures[ i ] );
			glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA16F, _windowWidth, _windowHeight, 0, GL_RGBA, GL_FLOAT, nullptr );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
			glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, _gBufferTextures[ i ], 0 );
			attachments[ i ] = GL_COLOR_ATTACHMENT0 + i;
		}
		glBindTexture( GL_TEXTURE_2D, _gBufferTextures[ 5 ] );
		glTexImage2D( GL_TEXTURE_2D,
					  0,
					  GL_DEPTH_COMPONENT,
					  _windowWidth,
					  _windowHeight,
					  0,
					  GL_DEPTH_COMPONENT,
					  GL_FLOAT,
					  nullptr );
		glFramebufferTexture2D( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, _gBufferTextures[ 5 ], 0 );
		glDrawBuffers( 5, attachments );
		if ( glCheckFramebufferStatus( GL_FRAMEBUFFER ) != GL_FRAMEBUFFER_COMPLETE )
		{
			throw std::runtime_error( "Framebuffer not complete!" );
		}
		glBindFramebuffer( GL_FRAMEBUFFER, 0 );
	}

	void LabWork6::_initGeometryPassProgram()
	{
		_geometryPassProgram
			= createProgramFromFiles( _shaderFolder + "geometry_pass.vert", _shaderFolder + "geometry_pass.frag" );
		if ( _geometryPassProgram == 0 )
		{
			throw std::runtime_error( "Failed to create geometry pass program!" );
		}
	}

	void LabWork6::_initShadingPassProgram()
	{
		_shadingPassProgram
			= createProgramFromFiles( _shaderFolder + "shading_pass.vert", _shaderFolder + "shading_pass.frag" );
		if ( _shadingPassProgram == 0 )
		{
			throw std::runtime_error( "Failed to create shading pass program!" );
		}
	}

	void LabWork6::_initFullScreenQuad()
	{
		float quadVertices[]
			= { -1.0f, 1.0f, 0.0f, 1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, -1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f };
		unsigned int indices[] = { 0, 1, 2, 0, 2, 3 };
		glCreateVertexArrays( 1, &_quadVAO );
		glCreateBuffers( 1, &_quadVBO );
		glCreateBuffers( 1, &_quadEBO );
		glNamedBufferData( _quadVBO, sizeof( quadVertices ), quadVertices, GL_STATIC_DRAW );
		glNamedBufferData( _quadEBO, sizeof( indices ), indices, GL_STATIC_DRAW );
		glVertexArrayVertexBuffer( _quadVAO, 0, _quadVBO, 0, 4 * sizeof( float ) );
		glVertexArrayElementBuffer( _quadVAO, _quadEBO );
		glEnableVertexArrayAttrib( _quadVAO, 0 );
		glVertexArrayAttribFormat( _quadVAO, 0, 2, GL_FLOAT, GL_FALSE, 0 );
		glVertexArrayAttribBinding( _quadVAO, 0, 0 );
		glEnableVertexArrayAttrib( _quadVAO, 1 );
		glVertexArrayAttribFormat( _quadVAO, 1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof( float ) );
		glVertexArrayAttribBinding( _quadVAO, 1, 0 );
	}

	void LabWork6::_geometryPass()
	{
		glBindFramebuffer( GL_FRAMEBUFFER, _gBufferFBO );
		glViewport( 0, 0, _windowWidth, _windowHeight );
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		glUseProgram( _geometryPassProgram );

		// ===  Envoie des matrices ===
		Mat4f modelMatrix  = Mat4f( 1.0f );
		Mat4f viewMatrix   = _camera.getViewMatrix();
		Mat4f projMatrix   = _camera.getProjectionMatrix();
		Mat4f mvMatrix	   = viewMatrix * modelMatrix;
		Mat4f mvpMatrix	   = projMatrix * mvMatrix;
		Mat3f normalMatrix = glm::transpose( glm::inverse( Mat3f( mvMatrix ) ) );

		glProgramUniformMatrix4fv( _geometryPassProgram,
								   glGetUniformLocation( _geometryPassProgram, "uMVP" ),
								   1,
								   GL_FALSE,
								   glm::value_ptr( mvpMatrix ) );
		glProgramUniformMatrix4fv( _geometryPassProgram,
								   glGetUniformLocation( _geometryPassProgram, "uModel" ),
								   1,
								   GL_FALSE,
								   glm::value_ptr( modelMatrix ) );
		glProgramUniformMatrix3fv( _geometryPassProgram,
								   glGetUniformLocation( _geometryPassProgram, "uNormalMatrix" ),
								   1,
								   GL_FALSE,
								   glm::value_ptr( normalMatrix ) );

		// === Rendu du modèle ===
		_model.render( _geometryPassProgram );

		glBindFramebuffer( GL_FRAMEBUFFER, 0 );

		if ( _displayDebugTexture )
		{
			glBindFramebuffer( GL_READ_FRAMEBUFFER, _gBufferFBO );	 // lecture dans le G-Buffer
			glReadBuffer( GL_COLOR_ATTACHMENT0 + _selectedTexture ); // choisir la texture cible

			glBindFramebuffer( GL_DRAW_FRAMEBUFFER, 0 ); // dessiner dans le framebuffer par défaut
			glBlitFramebuffer(
				0, 0, _windowWidth, _windowHeight, 0, 0, _windowWidth, _windowHeight, GL_COLOR_BUFFER_BIT, GL_NEAREST );
		}
	}

	void LabWork6::_shadingPass()
	{
		glClear( GL_COLOR_BUFFER_BIT );
		glDisable( GL_DEPTH_TEST );

		glUseProgram( _shadingPassProgram );

		// Bind les textures du G-Buffer
		for ( int i = 0; i < 5; ++i )
			glBindTextureUnit( i, _gBufferTextures[ i ] );

		// Envoyer la position de la lumière (uniforme uLightPosition)
		const GLint lightPosLoc = glGetUniformLocation( _shadingPassProgram, "uLightPosition" );
		glProgramUniform3fv( _shadingPassProgram, lightPosLoc, 1, glm::value_ptr( _lightPosition ) );

		// Dessin du quad plein écran
		glBindVertexArray( _quadVAO );
		glDrawElements( GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0 );

		glEnable( GL_DEPTH_TEST );
	}

	void LabWork6::_initCamera()
	{
		_camera.setPosition( Vec3f( 0.f, 1.f, 3.f ) );
		_camera.setScreenSize( _windowWidth, _windowHeight );
		_camera.setFovy( _fovy );
	}

	void LabWork6::handleEvents( const SDL_Event & p_event )
	{
		if ( p_event.type == SDL_KEYDOWN )
		{
			switch ( p_event.key.keysym.scancode )
			{
			case SDL_SCANCODE_W: // Front
				_camera.moveFront( _cameraSpeed );
				_updateViewMatrix();
				break;
			case SDL_SCANCODE_S: // Back
				_camera.moveFront( -_cameraSpeed );
				_updateViewMatrix();
				break;
			case SDL_SCANCODE_A: // Left
				_camera.moveRight( -_cameraSpeed );
				_updateViewMatrix();
				break;
			case SDL_SCANCODE_D: // Right
				_camera.moveRight( _cameraSpeed );
				_updateViewMatrix();
				break;
			case SDL_SCANCODE_R: // Up
				_camera.moveUp( _cameraSpeed );
				_updateViewMatrix();
				break;
			case SDL_SCANCODE_F: // Bottom
				_camera.moveUp( -_cameraSpeed );
				_updateViewMatrix();
				break;
			default: break;
			}
		}

		// Rotate when left click + motion (if not on Imgui widget).
		if ( p_event.type == SDL_MOUSEMOTION && p_event.motion.state & SDL_BUTTON_LMASK
			 && !ImGui::GetIO().WantCaptureMouse )
		{
			_camera.rotate( p_event.motion.xrel * _cameraSensitivity, p_event.motion.yrel * _cameraSensitivity );
			_updateViewMatrix();
		}
	}
	void LabWork6::resize( const int p_width, const int p_height )
	{
		BaseLabWork::resize( p_width, p_height );
		_camera.setScreenSize( p_width, p_height );
	}

	void LabWork6::_updateViewMatrix() {}

	void LabWork6::displayUI()
	{
		ImGui::Begin( "Deferred Shading Debug" );

		ImGui::Checkbox( "Afficher texture G-Buffer", &_displayDebugTexture );
		ImGui::SliderFloat3( "Light Position", &_lightPosition.x, -10.f, 10.f );

		static const char * items[] = { "Position", "Normal", "Ambient", "Diffuse", "Specular" };
		ImGui::Combo( "Texture to Display", &_selectedTexture, items, IM_ARRAYSIZE( items ) );

		if ( _displayDebugTexture )
		{
			ImGui::Text( "Apercu texture %s :", items[ _selectedTexture ] );
			ImGui::Image( reinterpret_cast<void *>( static_cast<intptr_t>( _gBufferTextures[ _selectedTexture ] ) ),
						  ImVec2( 256, 256 ),
						  ImVec2( 0, 1 ), // flip vertical
						  ImVec2( 1, 0 ) );
		}

		ImGui::End();
	}

	const std::string M3D_ISICG::LabWork6::_shaderFolder = "src/lab_works/lab_work_6/shaders/";

} // namespace M3D_ISICG