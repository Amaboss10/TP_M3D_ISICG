#ifndef __LAB_WORK_4_HPP__
#define __LAB_WORK_4_HPP__

#include "GL/gl3w.h"
#include "common/base_lab_work.hpp"
#include "common/camera.hpp"
#include "define.hpp"
#include <vector>
#include <map>
#include "common/models/triangle_mesh_model.hpp"

namespace M3D_ISICG
{
	class LabWork4 : public BaseLabWork
	{
	  public:
		// Construction
		LabWork4() = default;
		~LabWork4();

		// Méthodes principales
		bool init() override;
		void animate( const float ) override;
		void render() override;
		void handleEvents( const SDL_Event & ) override;
		void displayUI() override;
		void resize( const int, const int ) override;

	  private:
		// Méthodes internes
		void _updateViewMatrix();
		void _initCamera();
		void _resetLightingToDefault();
		void _loadScene( const std::string & name );

		// Composants
		Camera			  _camera;
		TriangleMeshModel _model;
		GLuint			  _program = GL_INVALID_INDEX;

		// Uniform locations
		GLint _uMVPMatrixLoc		= GL_INVALID_INDEX;
		GLint _uMVMatrixLoc			= GL_INVALID_INDEX;
		GLint _uViewMatrixLoc		= GL_INVALID_INDEX;
		GLint _uNormalMatrixLoc		= GL_INVALID_INDEX;
		GLint _uCameraPositionLoc	= GL_INVALID_INDEX;
		GLint _uLightPositionLoc	= GL_INVALID_INDEX;

		// Éclairage
		Vec3f _ambientColor	 = Vec3f( 0.2f, 0.2f, 0.2f );
		Vec3f _diffuseColor	 = Vec3f( 0.1f, 0.0f, 0.0f );
		Vec3f _specularColor = Vec3f( 1.0f, 1.0f, 1.0f );
		Vec3f _lightPosition = Vec3f( 0.f, 2.5f, 2.f );
		float _shininess	 = 32.0f;
		bool  _useBlinnPhong = true;

		// Caméra
		float _fovy				 = 60.f;
		float _cameraSpeed		 = 0.1f;
		float _cameraSensitivity = 0.1f;
		float _rotationAngle	 = 0.f;

		// Scènes
		int						 _currentSceneIndex = 0;
		std::vector<std::string> _sceneNames		= { "bunny", "conference" };

		// Fond
		Vec4f _bgColor = Vec4f( 0.2f, 0.2f, 0.2f, 1.f );

		// Shaders
		static const std::string _shaderFolder;
	};

} // namespace M3D_ISICG

#endif // __LAB_WORK_4_HPP__
