#include "SceneManager.h"
#include "ResourceManager.h"

using std::vector;
using std::ofstream;
using std::ifstream;
using std::ios;
using namespace util;

/*Main Loading*/

void SceneManager::saveScene(cstring file, const vector<Model*>& models)
{
	ofstream out(file, ios::binary);

	uint objSize = 0;//testing 

	//amount of models
	auto begin = out.tellp();
	out.write((char*)&objSize, sizeof(objSize));

	objSize = 0;
	//Model data
	for(auto& model : models)
		objSize += sendModel(out, model);

	auto resume = out.tellp();
	out.seekp(begin);
	out.write((char*)&objSize, sizeof(objSize));
	out.seekp(resume);

	out.close();
}

void SceneManager::loadScene(cstring file, std::list<Model>& models)
{
	ifstream in(file, ios::binary);

	if(!in.is_open()) { puts("Failed to load scene"); return; }

	uint objSize = 0;

	//amount of models
	in.read((char*)&objSize, sizeof(objSize));
	models.resize(objSize);

	//Model data
	uint count = 0;
	for(auto& model : models)
		count += recvModel(in, &model);

	in.close();

}


/*Sub-loading*/

bool SceneManager::sendTexture(std::ofstream& out, Texture2D& obj)
{
	uint dataSize;

	// texture Path
	dataSize = (uint)obj.path.size() + 1;
	out.write((char*)&dataSize, sizeof(uint));
	out.write((char*)obj.path.data(), dataSize);

	// texture name
	dataSize = (uint)obj.name.size() + 1;
	out.write((char*)&dataSize, sizeof(uint));
	out.write((char*)obj.name.data(), dataSize);

	//Texture Type
	{
		auto dat = obj.type;
		out.write((char*)&dat, sizeof(dat));
	}

	//Texture colour
	{
		auto dat = obj.colour;
		out.write((char*)&dat, sizeof(dat));
	}

	return true;
}

bool SceneManager::recvTexture(std::ifstream& in, Texture2D& obj)
{
	uint dataSize;

	// texture Path
	//dataSize = (uint)strlen(obj.path) + 1;
	in.read((char*)&dataSize, sizeof(uint));
	obj.path.resize(dataSize);
	in.read((char*)obj.path.data(), dataSize);

	// texture name
	// dataSize = (uint)strlen(obj.name) + 1;
	in.read((char*)&dataSize, sizeof(uint));
	obj.name.resize(dataSize);
	in.read((char*)obj.name.data(), dataSize);

	obj = ResourceManager::getTexture2D(obj.path.c_str());

	//Texture Type
	{
		in.read((char*)&obj.type, sizeof(obj.type));
	}

	//Texture colour
	{
		in.read((char*)&obj.colour, sizeof(obj.colour));
	}

	return true;
}


bool SceneManager::sendMesh(ofstream& out, Mesh* obj)
{
	if(!obj)return false;
	if(obj->getCompType() != Component::MESH)return false;

	unsigned  dataSize = 0;


	//Mesh Name (size of name length + 1 then string)
	dataSize = (uint)obj->meshName.length() + 1;
	out.write((char*)&dataSize, sizeof(unsigned));
	out.write((char*)obj->meshName.data(), dataSize);

	//Amount of Vertecies (size of unsigned)
	dataSize = (uint)obj->getUnpackedData().size();
	out.write((char*)&dataSize, sizeof(unsigned));

	//Store Vertex Data (size of vertex3D * vertex amount)
	out.write((char*)obj->getUnpackedData().data(), sizeof(Vertex3D) * dataSize);

	//Amount of Indicies (size of unsigned)
	dataSize = (uint)obj->getIndicieData().size();
	out.write((char*)&dataSize, sizeof(unsigned));

	//Indicie Data (size of unsigned * indice amount)
	out.write((char*)obj->getIndicieData().data(), sizeof(unsigned) * dataSize);

	//Amount of Materials (size of unsigned)
	dataSize = (uint)obj->matNames.size();
	out.write((char*)&dataSize, sizeof(unsigned));

	//Material Names
	for(auto& b : obj->matNames)
	{
		dataSize = (uint)b.length() + 1;
		out.write((char*)&dataSize, sizeof(uint));
		out.write((char*)b.data(), dataSize);
	}

	//Amount of Textures
	dataSize = obj->getTextures().size();
	out.write((char*)&dataSize, sizeof(unsigned));

	//Texture Info (may add replace textures later)
	for(auto& b : obj->getTextures())
		sendTexture(out, b);

	//Bounds Data 
	out.write((char*)&obj->top/*first bound*/, sizeof(Vec3) * 6);

	return true;
}

bool SceneManager::recvMesh(std::ifstream& in, Mesh* obj)
{
	if(!obj)return false;
	if(obj->getCompType() != Component::MESH)return false;

	uint  dataSize = 0;


	//Mesh Name (size of name length + 1 then string)
	//dataSize = (uint)obj->meshName.length() + 1;
	in.read((char*)&dataSize, sizeof(uint));
	obj->meshName.resize(dataSize);
	in.read((char*)obj->meshName.data(), dataSize);

	//Amount of Vertecies (size of unsigned)
	//dataSize = (uint)obj->getUnpackedData().size();
	in.read((char*)&dataSize, sizeof(uint));
	obj->getUnpackedData().resize(dataSize);

	//Store Vertex Data (size of vertex3D * vertex amount)
	in.read((char*)obj->getUnpackedData().data(), sizeof(Vertex3D) * dataSize);

	//Amount of Indicies (size of unsigned)
	//dataSize = (uint)obj->getIndicieData().size();
	in.read((char*)&dataSize, sizeof(uint));
	obj->getIndicieData().resize(dataSize);

	//Indicie Data (size of unsigned * indice amount)
	in.read((char*)obj->getIndicieData().data(), sizeof(uint) * dataSize);

	//Amount of Materials (size of unsigned)
	//dataSize = (uint)obj->matNames.size();
	in.read((char*)&dataSize, sizeof(unsigned));
	obj->matNames.resize(dataSize);

	//Material Names
	for(auto& b : obj->matNames)
	{
		//dataSize = (uint)b.length() + 1;
		in.read((char*)&dataSize, sizeof(uint));
		b.resize(dataSize);
		in.read((char*)b.data(), dataSize);
	}

	//Amount of Textures
	//dataSize = obj->getTextures().size();
	in.read((char*)&dataSize, sizeof(unsigned));
	obj->getTextures().resize(dataSize);
	obj->getReplaceTextures().resize(dataSize);

	//Texture Info (may add replace textures later)
	for(auto& b : obj->getTextures())
		recvTexture(in, b);

	//Bounds Data 
	in.read((char*)&obj->top/*first bound*/, sizeof(Vec3) * 6);

	obj->init();
	return true;
}


bool SceneManager::sendModel(ofstream& out, Model* obj)
{
	if(!obj)return false;

	if(obj->getCompType() != Component::MODEL)return false;

	out, obj;
	unsigned  objSize = 0;

	//model tag (size of name length + 1 then string)
	objSize = strlen(obj->getTag()) + 1;
	out.write((char*)&objSize, sizeof(objSize));
	out.write(obj->getTag(), objSize);

	//model path (size of name length + 1 then string)
	objSize = strlen(obj->getPath()) + 1;
	out.write((char*)&objSize, sizeof(objSize));
	out.write(obj->getPath(), objSize);


	//colour Data
	{
		auto dat = obj->Model::getColour();
		out.write((char*)&dat, sizeof(dat));
	}

	//Activators Data
	{
		auto dat = obj->Model::getActivatorData();
		out.write((char*)&dat, sizeof(dat));
	}

	//TODO: Animations go here

	//Mesh Data
	{
		//Ammount of Meshes (size of uint)
		auto loc = out.tellp();
		objSize = 0;
		out.write((char*)&objSize, sizeof(objSize));

		//Add Meshes
		uint count = 0;
		for(auto& mesh : obj->Model::getMeshList())
			count += sendMesh(out, mesh.get());

		//Actual amout of meshes
		auto resume = out.tellp();
		out.seekp(loc);
		out.write((char*)&count, sizeof(count));
		out.seekp(resume);
	}


	//bounds Data
	{
		auto dat = obj->Model::getBoundsData();
		out.write((char*)&dat, sizeof(dat));
	}


	//~Transformer Stuff~//

	//Transformation Data
	{
		auto dat = obj->Transformer::getTransformationData();
		out.write((char*)&dat, sizeof(dat));
	}

	//Activators Data
	{
		auto dat = obj->Transformer::getActivatorData();
		out.write((char*)&dat, sizeof(dat));
	}

	//Matrix Data
	{
		auto dat = obj->Transformer::getMatrixData();
		out.write((char*)&dat, sizeof(dat));
	}

	//Child Data
	{
		//Amount of children
		auto loc = out.tellp();
		{
			uint dat = 0;//testing this out
			out.write((char*)&dat, sizeof(dat));
		}


		//Add Children (ONLY Models for now) (also assumes objects can be null)
		uint count = 0;
		for(auto& model : obj->getChildren())
		{
			if(!model)
				continue;//nullptr
			++count;

			//Child type
			auto dat = model->getCompType();
			out.write((char*)&dat, sizeof(dat));
			switch(obj->getCompType())
			{
			case Component::MODEL:
				sendModel(out, (Model*)model);
				break;
				//TODO: add other objects
			case Component::TEXT:
				break;
			default:
				break;
			}
		}

		//Actual amount of children
		auto resume = out.tellp();
		out.seekp(loc);
		out.write((char*)&count, sizeof(uint));
		out.seekp(resume);
	}


	return true;
}

bool SceneManager::recvModel(std::ifstream& in, Model* obj)
{
	if(!obj)return false;

	if(obj->getCompType() != Component::MODEL)return false;

	uint  objSize = 0;

	//model tag (size of name length + 1 then string)
	{
		in.read((char*)&objSize, sizeof(objSize));
		cstring tmp = new char[objSize];
		in.read((char*)tmp, objSize);
		obj->setTag(tmp);
	}

	//model path (size of name length + 1 then string)
	{
		in.read((char*)&objSize, sizeof(objSize));
		cstring tmp = new char[objSize];
		in.read((char*)tmp, objSize);
		obj->setPath(tmp);
	}

	//colour Data
	{
		ColourRGBA dat;
		in.read((char*)&dat, sizeof(dat));
		obj->Model::setColour(dat);
	}

	//Activators Data
	{
		Model::ActivatorData dat;
		in.read((char*)&dat, sizeof(dat));
		obj->setActivatorData(dat);
	}

	//TODO: Animations go here


	//Ammount of Meshes (size of uint)
	//auto loc = in.tellp();
	//objSize = (uint)obj->Model::getMeshList().size();
	in.read((char*)&objSize, sizeof(objSize));

	//mesh Data
	//uint count = 0;
	for(uint count = 0; count < objSize; ++count)
		obj->Model::getMeshList().push_back(std::shared_ptr<Mesh>(new Mesh)),
		recvMesh(in, obj->Model::getMeshList()[count].get());

	////Actual amout of meshes
	//auto resume = in.tellp();
	//in.seekp(loc);
	//in.write((char*)&count, sizeof(count));
	//in.seekp(resume);


	//bounds Data
	{
		Model::BoundsData dat;
		in.read((char*)&dat, sizeof(dat));
		obj->setBoundsData(dat);
	}


	//~Transformer Stuff~//

	//Transformation Data
	{
		Transformer::TransformationData dat;
		in.read((char*)&dat, sizeof(dat));
		obj->Transformer::setTransformationData(dat);
	}

	//Activators Data
	{
		Transformer::ActivatorData dat;
		in.read((char*)&dat, sizeof(dat));
		obj->Transformer::setActivatorData(dat);
	}

	//Matrix Data
	{
		Transformer::MatrixData dat;
		in.read((char*)&dat, sizeof(dat));
		obj->Transformer::setMatrixData(dat);
	}

	//Child Data
	{
		//Amount of children
		in.read((char*)&objSize, sizeof(uint));


		//Add Children (ONLY Models for now) (also assumes no objects are null)
		for(uint a = 0; a < objSize; ++a)
		{
			//Child types
			Component::COMP_TYPE dat;
			in.read((char*)&dat, sizeof(dat));

			switch(dat)
			{
			case Component::MODEL:
			{
				Model* tmp;
				obj->getChildren().push_back(tmp = new Model());
				tmp->setParent(obj);
				recvModel(in, tmp);
			}
			break;
			//TODO: add other objects
			case Component::TEXT:
				break;
			default:
				break;
			}
		}
	}



	return true;
}