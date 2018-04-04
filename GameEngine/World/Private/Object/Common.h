#pragma once
#include "Core\Public\All.h"
#include "..\Geometry.h"

/*
Street�̎n�_�ƏI�_
    ��
1.Street����
2.������Intersection�ɔz�������邩, �V����Intersection�𐶐�
3.Intersection��Corner, Crosswalk�𐶐�

UniqueBuffer�@�{���I�ɕs�ςȏ��
ParameterBuffer   ���[�U�[���ύX�ł�����
TempBuffer�@�@�֋X��ꎞ�I�ɕێ�������

Design
�E���̗v�f���L�̃p�����[�^�[���Ǘ�����B
�E�f�t�H���g�p�����[�^�����肷��̂ɕK�v�ȗv�f��ێ�����B
�E���̃I�u�W�F�N�g�ƈˑ��������B

Detail
�E���̌`������肷��̂ɕK�v�ȑS�Ă̗v�f��ێ�����B
�E���̃I�u�W�F�N�g�ɂ͉e�����Ȃ��B

�p�����[�^�[����
�@FDesignStreet___________
�@      ��                ��
�@FDesignCorner �� FDesignCrosswalk�@

�|���S������
�@FDesignStreet 
�@      ��   
�@FDetailStreet      
�@      ��   ��___________
�@      |              |
�@DDesignCorner FDesignCrosswalk
�@      |   ________|  |
�@      ��   ��          ��           
�@FDetailCorner FDetailCrosswalk
*/

// �v���p�e�B�[�ǉ��p�}�N��
#define ParameterAddProperty(T,MIN,MAX) AddProperty(SProperty((void*)&T, #T, Typeid(decltype(T)).Name(), (float)(MIN), (float)(MAX)))

struct SProperty
{
	SProperty(void* ptr, const char* Name, const char* TypeName, float min, float max)
		:TargetPtr(ptr), Name(Name), TypeName(TypeName), MinValue(min), MaxValue(max)
	{
	}

	void* TargetPtr;
	const char* Name;
	const char* TypeName;
	float MinValue;
	float MaxValue;
};

struct SPropertySet
{
private:
	vector<SProperty> Properties;

public:
	void AddProperty(SProperty property)
	{
		Properties.push_back(property);
	}	
	

};

class FDetail
{
protected:
	vector<SVertex> Vertiecs;
	vector<uint> Indiecs;
	FRModel* Model;	

public:
	virtual FStartupShape* CreateShape() 
	{
		return nullptr;
	}

	void Update()
	{
		auto Shape = CreateShape();
		Model->Initialize(Shape->ToMesh());
	}
	
	void Render()
	{
		Model->Render();
	}
};

template<class TUnique, class TParameter, class TTemp>
class FDesign
{
private:
	vector<FDetail*> Details;

protected:
	TUnique Unique;
	TParameter Parameter;
	TTemp Temp;

	void OnParameterChanged()
	{
		Temp.Update(Unique, Parameter);
		for (auto detail : Details)detail->Update();
	}

public:
	void AddDetail(FDetail* detail) const
	{
		auto details = Details;
		details.push_back(detail);
	}
};