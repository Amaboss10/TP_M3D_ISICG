
#include "lab_work_3.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "imgui.h"
#include "common/base_mesh.hpp"
#include "common/shader_utils.hpp"
#include "utils/random.hpp"
#include "utils/read_file.hpp"
#include <iostream>

namespace M3D_ISICG
{
	const std::string LabWork3::_shaderFolder = "src/lab_works/lab_work_3/shaders/";

	LabWork3::~LabWork3() { _cube.destroy(); }

	bool LabWork3::init()
	{
		std::cout << "Initializing lab work 3 with camera..." << std::endl;

		_program = createProgramFromFiles( _shaderFolder + "lw3.vert", _shaderFolder + "lw3.frag" );
		if ( _program == 0 )
			return false;

		glClearColor( _bgColor.x, _bgColor.y, _bgColor.z, _bgColor.w );
		glEnable( GL_DEPTH_TEST );

		_createCube();
		_initCamera();

		glUseProgram( _program );
		return true;
	}

	void LabWork3::_initCamera()
	{	
		_camera.setScreenSize( _windowWidth, _windowHeight );
		_camera.setPosition( Vec3f( 0.f, 1.f, 3.f ) );
	
	}

	void LabWork3::_updateViewMatrix()
	{
		_uViewMatrixLoc = glGetUniformLocation( _program, "viewMatrix" );
		glProgramUniformMatrix4fv( _program, _uViewMatrixLoc, 1, GL_FALSE, &_camera.getViewMatrix()[ 0 ][ 0 ] );
	}

	void LabWork3::_updateProjectionMatrix()
	{
		_uProjectionMatrixLoc = glGetUniformLocation( _program, "uProjectionMatrix" );
		glProgramUniformMatrix4fv(
		_program, _uProjectionMatrixLoc, 1, GL_FALSE, &_camera.getProjectionMatrix()[ 0 ][ 0 ] );
	}

	void LabWork3::resize( const int p_width, const int p_height )
	{
		BaseLabWork::resize( p_width, p_height );
		_camera.setScreenSize( p_width, p_height );
	}

	void LabWork3::_createCube()
	{
		_cube.positions
			= { { -0.5f, -0.5f, -0.5f }, { 0.5f, -0.5f, -0.5f }, { 0.5f, 0.5f, -0.5f }, { -0.5f, 0.5f, -0.5f },
				{ -0.5f, -0.5f, 0.5f },	 { 0.5f, -0.5f, 0.5f },	 { 0.5f, 0.5f, 0.5f },	{ -0.5f, 0.5f, 0.5f } };

		_cube.colors = { { 1.f, 0.f, 0.f }, { 0.f, 1.f, 0.f }, { 0.f, 0.f, 1.f },	 { 1.f, 1.f, 0.f },
						 { 1.f, 0.f, 1.f }, { 0.f, 1.f, 1.f }, { 0.5f, 0.5f, 0.5f }, { 1.f, 1.f, 1.f } };

		_cube.indices = {
			0, 1, 2, 2, 3, 0, // back
			4, 5, 6, 6, 7, 4, // front
			0, 4, 7, 7, 3, 0, // left
			1, 5, 6, 6, 2, 1, // right
			3, 2, 6, 6, 7, 3, // top
			0, 1, 5, 5, 4, 0  // bottom
		};

		_cube.uploadToGPU();
	}

	void LabWork3::animate( const float p_deltaTime ) { _rotationAngle += p_deltaTime; }

	void LabWork3::handleEvents( const SDL_Event & p_event )
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

	void LabWork3::render()
	{
		// Nettoyage du frame buffer avec profondeur
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
		glUseProgram( _program );

		// Matrice modèle : mise à l’échelle 0.8 + rotation animée
		Mat4f model = glm::scale( glm::mat4( 1.f ), Vec3f( _cubeScale ) );
		model		= glm::rotate( model, _rotationAngle, Vec3f( 0.3f, 1.f, 0.2f ) );

		// Récupération des matrices View et Projection
		const Mat4f & view = _camera.getViewMatrix();
		const Mat4f & proj = _camera.getProjectionMatrix();

		// Localisation des uniforms
		GLuint modelLoc = glGetUniformLocation( _program, "modelMatrix" );
		GLuint viewLoc	= glGetUniformLocation( _program, "viewMatrix" );
		GLuint projLoc	= glGetUniformLocation( _program, "uProjectionMatrix" );

		// Envoi des matrices au shader
		glProgramUniformMatrix4fv( _program, modelLoc, 1, GL_FALSE, glm::value_ptr( model ) );
		glProgramUniformMatrix4fv( _program, viewLoc, 1, GL_FALSE, glm::value_ptr( view ) );
		glProgramUniformMatrix4fv( _program, projLoc, 1, GL_FALSE, glm::value_ptr( proj ) );

		// Dessin du cube
		_cube.draw();
	}

	void LabWork3::displayUI()
	{	
		ImGui::Begin( "Settings Lab Work 3" );

		// Background.
		if ( ImGui::ColorEdit3( "Background", glm::value_ptr( _bgColor ) ) )
		{
			glClearColor( _bgColor.x, _bgColor.y, _bgColor.z, _bgColor.w );
		}

		// Camera.
		if ( ImGui::SliderFloat( "fovy", &_fovy, 10.f, 160.f, "%01.f" ) )
		{
			_camera.setFovy( _fovy );
			_updateProjectionMatrix();
		}
		if ( ImGui::SliderFloat( "Speed", &_cameraSpeed, 0.1f, 10.f, "%01.1f" ) )
		{
			_camera.setFovy( _fovy );
			_updateProjectionMatrix();
		}

		// Échelle du cube
		ImGui::SliderFloat( "Scale", &_cubeScale, 0.1f, 2.f, "%.2f" );
		
		// Affichage de la position de la caméra
		Vec3f pos = _camera.getPos();
		ImGui::Text( "Camera pos: %.2f %.2f %.2f", pos.x, pos.y, pos.z );

		ImGui::End();
	}
} // namespace M3D_ISICG