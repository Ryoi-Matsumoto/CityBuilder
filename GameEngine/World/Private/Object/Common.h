#pragma once
#include "Core\Public\All.h"
#include "..\Geometry.h"

/*
Streetの始点と終点
    ↓
1.Street生成
2.既存のIntersectionに配属させるか, 新たなIntersectionを生成
3.IntersectionがCorner, Crosswalkを生成

UniqueBuffer　本質的に不変な情報
ParameterBuffer   ユーザーが変更できる情報
TempBuffer　　便宜上一時的に保持する情報

Design
・その要素特有のパラメーターを管理する。
・デフォルトパラメータを決定するのに必要な要素を保持する。
・他のオブジェクトと依存し合う。

Detail
・その形状を決定するのに必要な全ての要素を保持する。
・他のオブジェクトには影響しない。

パラメーター決定
　FDesignStreet___________
　      ↓                ↓
　FDesignCorner → FDesignCrosswalk　

ポリゴン生成
　FDesignStreet 
　      ↓   
　FDetailStreet      
　      ↑   ↑___________
　      |              |
　DDesignCorner FDesignCrosswalk
　      |   ________|  |
　      ↓   ↓          ↓           
　FDetailCorner FDetailCrosswalk
*/

// プロパティー追加用マクロ
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