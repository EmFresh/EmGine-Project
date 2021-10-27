//#include "Sprite.h"
//
//Shader * Sprite::_textureShader = new Shader;
//
//Sprite::Sprite()
//{
//	_vboID = 0;
//}
//
//Sprite::Sprite(VboInfo2D info) :VboInfo2D(info)
//{
//	_vboID = 0;
//	init();
//}
//
//Sprite::Sprite(VboInfo2D info, cstring * path) :VboInfo2D(info)
//{
//	_vboID = 0;
//	init(path);
//}
//
//Sprite::~Sprite()
//{
//	if(_vboID)
//		glDeleteBuffers(1, &_vboID);
//	
//}
//
//void Sprite::init()
//{
//	if(!_vboID)
//		glGenBuffers(1, &_vboID);
//
//	int w, h;
//	glfwGetFramebufferSize(glfwGetCurrentContext(),&w,&h);
//	size.width /= w;
//	size.height /= h;
//	position.x /= w;
//	position.y /= h;
//	//Left triangle
//	//bottom left
//	_vertData[0].setCoord2D(position.x - size.width / 2, position.y - size.height / 2);
//	_vertData[0].setUV(0, 0);
//	//top left
//	_vertData[1].setCoord2D(_vertData[0].coord[0], position.y + size.height / 2);
//	_vertData[1].setUV(0, 1);
//
//	//top right
//	_vertData[2].setCoord2D(_vertData[1].coord[0] + size.width, _vertData[1].coord[1]);
//	_vertData[2].setUV(1, 1);
//
//	//Right triangle 	
//	//top right
//	_vertData[3].setCoord2D(_vertData[2].coord[0], _vertData[2].coord[1]);
//	_vertData[3].setUV(1, 1);
//
//	//bottom right
//	_vertData[4].setCoord2D(_vertData[3].coord[0], _vertData[0].coord[1]);
//	_vertData[4].setUV(1, 0);
//
//	//bottom left
//	_vertData[5].setCoord2D(_vertData[0].coord[0], _vertData[0].coord[1]);
//	_vertData[5].setUV(0, 0);
//
//
//	glBindBuffer(GL_ARRAY_BUFFER, _vboID);
//	glBufferData(GL_ARRAY_BUFFER, sizeof(_vertData), _vertData, GL_DYNAMIC_DRAW);
//	glBindBuffer(GL_ARRAY_BUFFER, 0);
//}
//
//void Sprite::init(cstring * path)
//{
//	static bool initial;
//
//	if(!initial)
//	{
//		printf("using texture shader\n");
//		_textureShader->compileShaders("Shaders/Texture.vtsh", "Shaders/Texture.fmsh");
//		_textureShader->linkShaders();
//		//for(int a = 0; a < 6; a++)
//		//	_vertData[a].setColour(255, 255, 255);
//
//	}
//	initial = true;
//
//	init();
//	_texture = ResourceManager::getTexture2D(path);
//}
//
//void Sprite::draw()
//{
//	_textureShader->enable();
//
//	glActiveTexture(GL_TEXTURE0);
//	//printf("ID: %d", _texture.id);
//	if(_texture.id != _bound)
//	{
//		glBindTexture(GL_TEXTURE_2D, _texture.id);
//		_bound = _texture.id;
//	}
//	glUniform1i(_textureShader->getUniformLocation("texSampler"), 0);
//
//	glBindBuffer(GL_ARRAY_BUFFER, _vboID);
//
//	glEnableVertexAttribArray(0);
//	glEnableVertexAttribArray(1);
//	glEnableVertexAttribArray(2);
//
//	//vertex atributes
//	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex2D), (void*)offsetof(Vertex2D, coord));
//	//ColourRGBA atributes
//	glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex2D), (void*)offsetof(Vertex2D, colour));
//	//UV     atributes
//	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex2D), (void*)offsetof(Vertex2D, uv));
//
//
//	glDrawArrays(GL_TRIANGLES, 0, 6);
//
//	glDisableVertexAttribArray(0);
//	glDisableVertexAttribArray(1);
//	glDisableVertexAttribArray(2);
//
//	glBindBuffer(GL_ARRAY_BUFFER, 0);
//
//	//glBindTexture(GL_TEXTURE_2D, 0);
//	_textureShader->disable();
//}
//
//void Sprite::translate(float x, float y)
//{
//	position = {x,y};
//
//	init();
//}
//
//void Sprite::setSize(float width, float height)
//{
//	
//	size = {width,height};
//
//	init();
//}
//
//void Sprite::rotate(float deg)
//{
//
//	static Quat quat[6];
//	static bool init;
//	if(!_vboID)
//		glGenBuffers(1, &_vboID);
//
//	for(int a = 0; a < 6; a++)
//	{
//		_vertData[a].coord -= {position.x, position.y};
//		quat[a] = {false,_vertData[a].coord.x,_vertData[a].coord.y,0};
//
//		if(!init)
//			_vertData[a].print();
//
//		quat[a].rotate(-_angle, 0, 0, 1);
//		quat[a].rotate(deg, 0, 0, 1);
//		_vertData[a].setCoord2D(quat[a].x, quat[a].x);
//
//		if(!init)
//		{
//			_vertData[a].print();
//
//		}
//		_vertData[a].coord += {position.x, position.y};
//	}
//	if(deg == _angle)
//		init = true;
//	else
//		printf("\n\n");
//	_angle = deg;
//
//
//
//
//	glBindBuffer(GL_ARRAY_BUFFER, _vboID);
//	glBufferData(GL_ARRAY_BUFFER, sizeof(_vertData), _vertData, GL_DYNAMIC_DRAW);
//	glBindBuffer(GL_ARRAY_BUFFER, 0);
//}
//
//void Sprite::rotateBy(float deg)
//{
//	deg;
//}
//
//Coord2D<>& Sprite::getSize()
//{
//	return size;
//}