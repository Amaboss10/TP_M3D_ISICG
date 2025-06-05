#ifndef __LAB_WORK_5_HPP__
#define __LAB_WORK_5_HPP__

#include "GL/gl3w.h"
#include "common/base_lab_work.hpp"
#include "common/camera.hpp"
#include "common/models/triangle_mesh_model.hpp"
#include "define.hpp"

namespace M3D_ISICG
{
	class LabWork5 : public BaseLabWork
	{
	  public:
		LabWork5() = default;
		~LabWork5();

		bool init() override;
		void animate( const float ) override;
		void render() override;
		void handleEvents( const SDL_Event & ) override;
		void displayUI() override;
		void resize( const int, const int ) override;

	  private:
		void _initCamera();
		void _updateViewMatrix(); 
		void _loadScene( const std::string & name ); 

		Camera			  _camera; 
		TriangleMeshModel _model; 

		GLuint _program = GL_INVALID_INDEX; 

		// === Uniform locations === 
		GLint _uMVPMatrixLoc	  = GL_INVALID_INDEX; 
		GLint _uMVMatrixLoc		  = GL_INVALID_INDEX; 
		GLint _uViewMatrixLoc	  = GL_INVALID_INDEX;
		GLint _uNormalMatrixLoc	  = GL_INVALID_INDEX;
		GLint _uCameraPositionLoc = GL_INVALID_INDEX;
		GLint _uLightPositionLoc  = GL_INVALID_INDEX;

		// Pour gérer dynamiquement les textures (booléens uHas*)
		GLint _uHasDiffuseMapLoc   = GL_INVALID_INDEX;
		GLint _uHasSpecularMapLoc  = GL_INVALID_INDEX;
		GLint _uHasShininessMapLoc = GL_INVALID_INDEX;
		GLint _uHasNormalMapLoc	   = GL_INVALID_INDEX;

		// === Lumière et matériau ===
		Vec3f _ambientColor	 = Vec3f( 0.f, 0.f, 0.f ); 
		Vec3f _diffuseColor	 = Vec3f( 0.f, 0.f, 0.f ); 
		Vec3f _specularColor = Vec3f( 0.f, 0.f, 0.f ); 
		float _shininess	 = 32.0f; 
		Vec3f _lightPosition = Vec3f( 0.f, 3.f, 2.f );
		bool _useBlinnPhong = true;

		// === Caméra ===
		Vec4f _bgColor			 = Vec4f( 0.2f, 0.2f, 0.2f, 1.f );
		float _fovy				 = 60.f;
		float _cameraSpeed		 = 0.1f;
		float _cameraSensitivity = 0.1f;
		float _rotationAngle	 = 0.f;

		//===scène===
		std::vector<std::string> _sceneNames		= { "bunny", "sponza" };
		int						 _currentSceneIndex = 1;
		bool					 _showModel			= true;
		bool					 _showSkybox		= true;

		static const std::string _shaderFolder;
	};
} // namespace M3D_ISICG

#endif // __LAB_WORK_5_HPP__
