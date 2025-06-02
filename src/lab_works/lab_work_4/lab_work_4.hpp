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
		LabWork4() = default;
		~LabWork4();

		bool init() override;
		void animate( const float p_deltaTime ) override;
		void render() override;

		void handleEvents( const SDL_Event & p_event ) override;
		void displayUI() override;

		void resize( const int p_width, const int p_height ) override;
	  
	  private: 
		void _updateViewMatrix();
		void _initCamera();
		void _resetLightingToDefault();


	  private:
		Camera					 _camera;
		TriangleMeshModel		 _model;

		GLuint					 _program			   = GL_INVALID_INDEX;
		GLint  _uMVPMatrixLoc = GL_INVALID_INDEX;
		GLint  _uMVMatrixLoc  = GL_INVALID_INDEX;

		GLint					 _uModelMatrixLoc	   = -1;
		GLint					 _uViewMatrixLoc	   = -1;
		GLint					 _uProjectionMatrixLoc = -1;
		GLint					 _uNormalMatrixLoc	   = GL_INVALID_INDEX;
		GLint					 _uCameraPositionLoc   = GL_INVALID_INDEX;
		GLint					 _uLightPositionLoc	   = GL_INVALID_INDEX;


		Vec4f					 _bgColor = Vec4f( 0.2f, 0.2f, 0.2f, 1.f );
		Vec3f _ambientColor	 = Vec3f( 0.2f, 0.2f, 0.2f );
		Vec3f _diffuseColor	 = Vec3f( 0.2f, 0.4f, 1.0f ); 
		Vec3f _specularColor = Vec3f( 1.0f, 1.0f, 1.0f );
		float _shininess	 = 32.0f;
		Vec3f					 _lightPosition = Vec3f( 1.0f, 1.2f, 1.5f );

		float					 _rotationAngle		= 0.f;
		float					 _cameraSpeed		= 0.1f;
		float					 _cameraSensitivity = 0.1f;
		float					 _fovy				= 60.f;

		static const std::string _shaderFolder;
	};
} // namespace M3D_ISICG

#endif // __LAB_WORK_4_HPP__
