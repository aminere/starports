/* 

Amine Rehioui
Created: April 2nd 2011

*/

#include "Shoot.h"

#include "File.h"

#include "rapidxml.hpp"
#include "rapidxml_print.hpp"

namespace shoot
{
	//! static variables initialization
	int Object::m_TypeCount = 0;

	DEFINE_OBJECT(Object);

	//! Constructor
	Object::Object()
		: m_ID(0)
	{
		ObjectManager::Instance()->RegisterObject(this);
	}

	//! Destructor
	Object::~Object()
	{
		ObjectManager::Instance()->UnregisterObject(this);
	}

	//! serializes the object to/from a PropertyStream
	void Object::Serialize(PropertyStream& stream)
	{
		#ifdef SHOOT_EDITOR
			if (stream.GetMode() == SM_Read)
				stream.SetTargetObject(this);
		#endif

		stream.Serialize("ID", &m_ID);
		stream.Serialize("Name", &m_strName);
	}

	//! Loads the object from XML
	void Object::LoadFromXML(rapidxml::xml_node<char>* pRoot)
	{
		PropertyStream stream(SM_Read, this);
		stream.ReadFromXML(pRoot);
		Serialize(stream);
	}

	//! returns the object's template path
	const std::string& Object::GetTemplatePath() const
	{
		return ObjectManager::Instance()->GetTemplatePath(const_cast<Object*>(this));
	}

	//! create a copy of the object
	Object* Object::Copy() const
	{
		return ObjectManager::Instance()->CopyObject(const_cast<Object*>(this));
	}

	//! Saves the object to XML
	void Object::SaveToXML(const char* strPath)
	{
		rapidxml::xml_document<> doc;
		auto root = doc.allocate_node(rapidxml::node_element, GetClassName());		
		doc.append_node(root);

		PropertyStream stream(SM_Write);
		Serialize(stream);

		stream.WriteToXML(root);
		
		std::string xml_as_string;
		rapidxml::print(std::back_inserter(xml_as_string), doc);
		auto file = File::Create(strPath, shoot::File::M_Write);
		if(file->Open())
		{
			file->Write(xml_as_string.c_str(), (int)xml_as_string.length());
			file->Close();
		}
		sdelete(file);
	}
}
