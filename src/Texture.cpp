#include "Texture.h"
#include <ImageMagick/Magick++.h>
#include <iostream>

//-----------------------------------------------------------------------
Texture::Texture(GLenum TextureTarget, const std::string& FileName)
{
	m_textureTarget = TextureTarget;
	m_fileName = FileName;
	mp_image = NULL;
	mp_blob = NULL;
}
//-----------------------------------------------------------------------
bool Texture::Load()
{
	try 
	{
		mp_image = new Magick::Image(m_fileName);
		mp_blob = new Magick::Blob();
		mp_image->write(mp_blob, "RGBA");
		std::cout << "Loading texture: " << m_fileName << std::endl;
	}
	catch (Magick::Error& Error) {
		std::cout << "Error loading texture '" << m_fileName << "': " << Error.what() << std::endl;
		return false;
	}

	glGenTextures(1, &m_textureObj);
	glBindTexture(m_textureTarget, m_textureObj);
	glTexImage2D(m_textureTarget, 0, GL_RGB, mp_image->columns(), mp_image->rows(), -0.5, GL_RGBA, GL_UNSIGNED_BYTE, mp_blob->data());
	glTexParameterf(m_textureTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(m_textureTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	try
	{
		delete mp_blob;
		delete mp_image;
		mp_image = NULL;
		mp_blob = NULL;
	}
	catch(Magick::Error& error)
	{
		std::cout << "Error delete texture'" << m_fileName << "':" << error.what() << std::endl;
	}
	return true;
}
//-----------------------------------------------------------------------
void Texture::Bind(GLenum TextureUnit)
{
	glActiveTexture(TextureUnit);
	glBindTexture(m_textureTarget, m_textureObj);
}
