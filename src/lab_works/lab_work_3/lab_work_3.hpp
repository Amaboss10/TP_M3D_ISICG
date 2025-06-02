#ifndef __LAB_WORK_3_HPP__
#define __LAB_WORK_3_HPP__

#include "GL/gl3w.h"
#include "common/base_lab_work.hpp"
#include "common/base_mesh.hpp"
#include "common/camera.hpp"
#include "define.hpp"
#include <vector>

namespace M3D_ISICG
{
	class LabWork3 : public BaseLabWork
	{
	  public:
		LabWork3() = default;
		~LabWork3();

		bool init() override;
		void animate( const float p_deltaTime ) override;
		void render() override;
		void handleEvents( const SDL_Event & p_event ) override;
		void displayUI() override;
		void resize( const int p_width, const int p_height ) override;

	  private:
		void _createCube();
		void _initCamera();
		void _updateViewMatrix();
		void _updateProjectionMatrix();

		GLuint	 _program			   = GL_INVALID_INDEX;
		GLint	 _uModelMatrixLoc	   = GL_INVALID_INDEX;
		GLint	 _uViewMatrixLoc	   = GL_INVALID_INDEX;
		GLint	 _uProjectionMatrixLoc = GL_INVALID_INDEX;

		BaseMesh _cube;
		Camera	 _camera;
		Vec4f	 _bgColor = Vec4f( 0.8f, 0.8f, 0.8f, 1.f );

		float _rotationAngle	 = 0.f;
		float _cameraSpeed		 = 0.1f;
		float _cameraSensitivity = 0.1f;
		float _fovy				 = 60.f;
		float _cubeScale = 0.4f;

		static const std::string _shaderFolder;
	};
} // namespace M3D_ISICG

#endif // __LAB_WORK_3_HPP__
