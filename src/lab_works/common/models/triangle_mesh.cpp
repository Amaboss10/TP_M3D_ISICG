#include "triangle_mesh.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <iostream>

namespace M3D_ISICG
{
	TriangleMesh::TriangleMesh( const std::string &				  p_name,
								const std::vector<Vertex> &		  p_vertices,
								const std::vector<unsigned int> & p_indices,
								const Material &				  p_material ) :
		_name( p_name ),
		_vertices( p_vertices ), _indices( p_indices ), _material( p_material )
	{
		_vertices.shrink_to_fit();
		_indices.shrink_to_fit();
		_setupGL();
	}

	void TriangleMesh::render( const GLuint p_program ) const
	{
		glUseProgram( p_program );

		// Material properties
		glUniform3fv( glGetUniformLocation( p_program, "ambientColor" ), 1, glm::value_ptr( _material._ambient ) );
		glUniform3fv( glGetUniformLocation( p_program, "difusColor" ), 1, glm::value_ptr( _material._diffuse ) );
		glUniform3fv( glGetUniformLocation( p_program, "speculaireColor" ), 1, glm::value_ptr( _material._specular ) );
		glUniform1f( glGetUniformLocation( p_program, "shininess" ), _material._shininess );

		// Send texture presence flags
		glUniform1i( glGetUniformLocation( p_program, "uHasDiffuseMap" ), _material._hasDiffuseMap );
		glUniform1i( glGetUniformLocation( p_program, "uHasAmbientMap" ), _material._hasAmbientMap );
		glUniform1i( glGetUniformLocation( p_program, "uHasSpecularMap" ), _material._hasSpecularMap );
		glUniform1i( glGetUniformLocation( p_program, "uHasShininessMap" ), _material._hasShininessMap );

		// Bind textures
		if ( _material._hasDiffuseMap )
			glBindTextureUnit( 1, _material._diffuseMap._id );

		if ( _material._hasAmbientMap )
			glBindTextureUnit( 2, _material._ambientMap._id );

		if ( _material._hasSpecularMap )
			glBindTextureUnit( 3, _material._specularMap._id );

		if ( _material._hasShininessMap )
			glBindTextureUnit( 4, _material._shininessMap._id );

		// Draw mesh
		glBindVertexArray( _vao );
		glDrawElements( GL_TRIANGLES, static_cast<GLsizei>( _indices.size() ), GL_UNSIGNED_INT, 0 );
		glBindVertexArray( 0 );

		// Cleanup texture units
		if ( _material._hasDiffuseMap )
			glBindTextureUnit( 1, 0 );
		if ( _material._hasAmbientMap )
			glBindTextureUnit( 2, 0 );
		if ( _material._hasSpecularMap )
			glBindTextureUnit( 3, 0 );
		if ( _material._hasShininessMap )
			glBindTextureUnit( 4, 0 );

		glUseProgram( 0 );
	}



	void TriangleMesh::cleanGL()
	{
		glDisableVertexArrayAttrib( _vao, 0 );
		glDisableVertexArrayAttrib( _vao, 1 );
		glDisableVertexArrayAttrib( _vao, 2 );
		glDisableVertexArrayAttrib( _vao, 3 );
		glDisableVertexArrayAttrib( _vao, 4 );
		glDeleteVertexArrays( 1, &_vao );
		glDeleteBuffers( 1, &_vbo );
		glDeleteBuffers( 1, &_ebo );
	}

	void TriangleMesh::_setupGL()
	{
		// === Création du VBO (vertex buffer)
		glCreateBuffers( 1, &_vbo );
		glNamedBufferData( _vbo, _vertices.size() * sizeof( Vertex ), _vertices.data(), GL_STATIC_DRAW );

		// === Création du EBO (element/index buffer)
		glCreateBuffers( 1, &_ebo );
		glNamedBufferData( _ebo, _indices.size() * sizeof( unsigned int ), _indices.data(), GL_STATIC_DRAW );

		// === Création du VAO (vertex array)
		glCreateVertexArrays( 1, &_vao );

		// Lier le VBO au VAO (binding index 0)
		glVertexArrayVertexBuffer( _vao, 0, _vbo, 0, sizeof( Vertex ) );

		// === Attribut 0 : Position
		glEnableVertexArrayAttrib( _vao, 0 );
		glVertexArrayAttribFormat( _vao, 0, 3, GL_FLOAT, GL_FALSE, offsetof( Vertex, _position ) );
		glVertexArrayAttribBinding( _vao, 0, 0 );

		// === Attribut 1 : Normale
		glEnableVertexArrayAttrib( _vao, 1 );
		glVertexArrayAttribFormat( _vao, 1, 3, GL_FLOAT, GL_FALSE, offsetof( Vertex, _normal ) );
		glVertexArrayAttribBinding( _vao, 1, 0 );

		// === Attribut 2 : Coordonnées de texture (UV)
		glEnableVertexArrayAttrib( _vao, 2 );
		glVertexArrayAttribFormat( _vao, 2, 2, GL_FLOAT, GL_FALSE, offsetof( Vertex, _texCoords ) );
		glVertexArrayAttribBinding( _vao, 2, 0 );

		// === Attribut 3 : Tangente
		glEnableVertexArrayAttrib( _vao, 3 );
		glVertexArrayAttribFormat( _vao, 3, 3, GL_FLOAT, GL_FALSE, offsetof( Vertex, _tangent ) );
		glVertexArrayAttribBinding( _vao, 3, 0 );

		// === Attribut 4 : Bitangente
		glEnableVertexArrayAttrib( _vao, 4 );
		glVertexArrayAttribFormat( _vao, 4, 3, GL_FLOAT, GL_FALSE, offsetof( Vertex, _bitangent ) );
		glVertexArrayAttribBinding( _vao, 4, 0 );

		// === Lier le EBO (indices) au VAO
		glVertexArrayElementBuffer( _vao, _ebo );
	}

} // namespace M3D_ISICG
