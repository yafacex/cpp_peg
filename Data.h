#pragma once
#include <string>
#include <sstream>
#include "Log.h"
using namespace std;
namespace yfpeg {
	class Data {
	public:
		Data() : ref(1) {};
		void retain() {
			ref++;
		};
		void release() {
			ref--;
			if (ref == 0) {
				delete this;
			}
		};
		virtual Data* clone() = 0;
		virtual string description() = 0;
		int ref;
	};
	class Object {
	public:
		Object(Data* _data):data(_data),ref(1){};
		Data* data;
		void retain() {
			ref++;
		};
		void release() {
			ref--;
			if (ref == 0) {
				delete this;
			}
		};
		int ref;
	};
	class Null :public Data {
	public:
		Null() :Data() {
		};
		Null(Null& v) :Data() {
		};
		virtual string description() {
			return "Null";
		};
		virtual Data* clone() {
			return new Null();
		};
	};
	class NumberInt :public Data {
	public:
		NumberInt(int v = 0) :Data() {
			value = v;
		};
		NumberInt(NumberInt& v) :Data() {
			value = v.value;
		};
		virtual string description() {
			return "NumberInt:" + int2str(value);
		};
		virtual Data* clone() {
			return new NumberInt(value);
		};
		int value;
	};
	class NumberFloat :public Data {
	public:
		NumberFloat(float v = 0.0f) :Data() {
			value = v;
		};
		NumberFloat(NumberFloat& v) :Data() {
			value = v.value;
		};
		virtual string description() {
			return "NumberFloat:" + float2str(value);
		};
		virtual Data* clone() {
			return new NumberFloat(value);
		};
		float value;
	};
	class String :public Data {
	public:
		String(string& v) :Data() {
			value = v;
		};
		String(String& v) :Data() {
			value = v.value;
		};
		virtual string description() {
			return "NumberFloat:" + value;
		};
		virtual Data* clone() {
			return new String(value);
		};
		string value;
	};
	//vectors
	class Vec2 :public Data {
	public:
		Vec2(float x = 0.0,float y = 0.0) :Data() {
			value_x = x;
			value_y = y;
		};
		Vec2(const Vec2& v) :Data() {
			value_x = v.value_x;
			value_y = v.value_y;
		};
		virtual string description() {
			return "Vec2:(" + float2str(value_x) + "," + float2str(value_y) + ")";
		};
		virtual Data* clone() {
			return new Vec2(value_x,value_y);
		};
		float value_x;
		float value_y;
		Vec2* operator+(Vec2* rvalue) {
			return new Vec2(value_x + rvalue->value_x, value_y + rvalue->value_y);
		};
		Vec2* operator-(Vec2* rvalue) {
			return new Vec2(value_x - rvalue->value_x, value_y - rvalue->value_y);
		};
		Vec2* operator*(float rvalue) {
			return new Vec2(value_x * rvalue, value_y * rvalue);
		};
		Vec2* operator/(float rvalue) {
			float inv = 1.0f / rvalue;
			return new Vec2(value_x * inv, value_y * inv);
		};
		float dot(Vec2* rvalue) {
			return value_x * rvalue->value_x + value_y * rvalue->value_y;
		}
		float cross(Vec2* rvalue) {
			return (value_x * rvalue->value_y - value_y * rvalue->value_x);
		}
	};
	class Vec3 :public Data {
	public:
		Vec3(float x = 0.0, float y = 0.0, float z = 0.0) :Data() {
			value_x = x;
			value_y = y;
			value_z = z;
		};
		Vec3(Vec3& v) :Data() {
			value_x = v.value_x;
			value_y = v.value_y;
			value_z = v.value_z;
		};
		virtual Data* clone() {
			return new Vec3(value_x,value_y,value_z);
		};
		virtual string description() {
			return "Vec3:" + float2str(value_x) + "," + float2str(value_y) + "," + float2str(value_z);
		};
		Vec3* operator+(Vec3* rvalue) {
			return new Vec3(value_x + rvalue->value_x, value_y + rvalue->value_y, value_z + rvalue->value_z);
		};
		Vec3* operator-(Vec3* rvalue) {
			return new Vec3(value_x - rvalue->value_x, value_y - rvalue->value_y, value_z - rvalue->value_z);
		};
		Vec3* operator*(float rvalue) {
			return new Vec3(value_x * rvalue, value_y * rvalue, value_z * rvalue);
		};
		Vec3* operator/(float rvalue) {
			float inv = 1.0f / rvalue;
			return new Vec3(value_x * inv, value_y * inv, value_z * inv);
		};
		float dot(Vec3* rvalue) {
			return value_x * rvalue->value_x + value_y * rvalue->value_y + value_z * rvalue->value_z;
		};
		Vec3* cross(Vec3* rvalue) {
			return new Vec3(
				value_y * rvalue->value_z - value_z * rvalue->value_y,
				value_z * rvalue->value_x - value_x * rvalue->value_z,
				value_x * rvalue->value_y - value_y * rvalue->value_x
			);
		};
		float value_x;
		float value_y;
		float value_z;
	};
	class Vec4 :public Data {
	public:
		Vec4(float x = 0.0, float y = 0.0, float z = 0.0, float w = 0.0) :Data() {
			value_x = x;
			value_y = y;
			value_z = z;
			value_w = w;
		};
		Vec4(Vec4& v) :Data() {
			value_x = v.value_x;
			value_y = v.value_y;
			value_z = v.value_z;
			value_w = v.value_w;
		};
		virtual Data* clone() {
			return new Vec4(value_x,value_y,value_z,value_w);
		};
		virtual string description() {
			return "Vec4:" + float2str(value_x) + "," + float2str(value_y) + "," + float2str(value_z) + "," + float2str(value_w);
		};
		Vec4* operator+(Vec4* rvalue) {
			return new Vec4(value_x + rvalue->value_x, value_y + rvalue->value_y, value_z + rvalue->value_z, value_w + rvalue->value_w);
		};
		Vec4* operator-(Vec4* rvalue) {
			return new Vec4(value_x - rvalue->value_x, value_y - rvalue->value_y, value_z - rvalue->value_z, value_w - rvalue->value_w);
		};
		Vec4* operator*(float rvalue) {
			return new Vec4(value_x * rvalue, value_y * rvalue, value_z * rvalue, value_w * rvalue);
		};
		Vec4* operator/(float rvalue) {
			float inv = 1.0f / rvalue;
			return new Vec4(value_x * inv, value_y * inv, value_z * inv, value_w * inv);
		};
		float dot(Vec4* rvalue) {
			return value_x * rvalue->value_x + value_y * rvalue->value_y + value_z * rvalue->value_z + value_w * rvalue->value_w;
		};
		Vec4* cross(Vec4* rvalue) {
			return new Vec4(0.0f, 0.0f, 0.0f, 0.0f);
		};
		float value_x;
		float value_y;
		float value_z;
		float value_w;
	};
	//ivectors
	class IVec2 :public Data {
	public:
		IVec2(int x = 0.0,int y = 0.0) :Data() {
			value_x = x;
			value_y = y;
		};
		IVec2(IVec2& v) :Data() {
			value_x = v.value_x;
			value_y = v.value_y;
		};
		virtual string description() {
			return "IVec2:" + int2str(value_x) + "," + int2str(value_y) ;
		};
		virtual Data* clone() {
			return new IVec2(value_x,value_y);
		};
		int value_x;
		int value_y;
		IVec2* operator+(IVec2* rvalue) {
			return new IVec2(int(value_x + rvalue->value_x), int(value_y + rvalue->value_y));
		};
		IVec2* operator-(IVec2* rvalue) {
			return new IVec2(int(value_x - rvalue->value_x), int(value_y - rvalue->value_y));
		};
		IVec2* operator*(float rvalue) {
			return new IVec2(int(value_x * rvalue), int(value_y * rvalue));
		};
		IVec2* operator/(float rvalue) {
			float inv = 1.0f / rvalue;
			return new IVec2(int(value_x * inv), int(value_y * inv));
		};
		float dot(IVec2* rvalue) {
			return float(value_x * rvalue->value_x + value_y * rvalue->value_y);
		}
		float cross(IVec2* rvalue) {
			return float(value_x * rvalue->value_y - value_y * rvalue->value_x);
		}
	};
	class IVec3 :public Data {
	public:
		IVec3(int x = 0, int y = 0, int z = 0) :Data() {
			value_x = x;
			value_y = y;
			value_z = z;
		};
		IVec3(IVec3& v) :Data() {
			value_x = v.value_x;
			value_y = v.value_y;
			value_z = v.value_z;
		};
		virtual string description() {
			return "IVec2:" + int2str(value_x) + "," + int2str(value_y) + "," + int2str(value_z);
		};
		virtual Data* clone() {
			return new IVec3(value_x,value_y,value_z);
		};
		IVec3* operator+(IVec3* rvalue) {
			return new IVec3(value_x + rvalue->value_x, value_y + rvalue->value_y, value_z + rvalue->value_z);
		};
		IVec3* operator-(IVec3* rvalue) {
			return new IVec3(value_x - rvalue->value_x, value_y - rvalue->value_y, value_z - rvalue->value_z);
		};
		IVec3* operator*(float rvalue) {
			return new IVec3(value_x * rvalue, value_y * rvalue, value_z * rvalue);
		};
		IVec3* operator/(float rvalue) {
			float inv = 1.0f / rvalue;
			return new IVec3(value_x * inv, value_y * inv, value_z * inv);
		};
		float dot(IVec3* rvalue) {
			return value_x * rvalue->value_x + value_y * rvalue->value_y + value_z * rvalue->value_z;
		};
		IVec3* cross(IVec3* rvalue) {
			return new IVec3(
				value_y * rvalue->value_z - value_z * rvalue->value_y,
				value_z * rvalue->value_x - value_x * rvalue->value_z,
				value_x * rvalue->value_y - value_y * rvalue->value_x
			);
		};
		int value_x;
		int value_y;
		int value_z;
	};
	class IVec4 :public Data {
	public:
		IVec4(int x = 0, int y = 0, int z = 0, int w = 0) :Data() {
			value_x = x;
			value_y = y;
			value_z = z;
			value_w = w;
		};
		IVec4(IVec4& v) :Data() {
			value_x = v.value_x;
			value_y = v.value_y;
			value_z = v.value_z;
			value_w = v.value_w;
		};
		virtual Data* clone() {
			return new IVec4(value_x,value_y,value_z,value_w);
		};
		virtual string description() {
			return "IVec2:" + int2str(value_x) + "," + int2str(value_y) + "," + int2str(value_z) + "," + int2str(value_w);
		};
		IVec4* operator+(IVec4* rvalue) {
			return new IVec4(value_x + rvalue->value_x, value_y + rvalue->value_y, value_z + rvalue->value_z, value_w + rvalue->value_w);
		};
		IVec4* operator-(IVec4* rvalue) {
			return new IVec4(value_x - rvalue->value_x, value_y - rvalue->value_y, value_z - rvalue->value_z, value_w - rvalue->value_w);
		};
		IVec4* operator*(float rvalue) {
			return new IVec4(value_x * rvalue, value_y * rvalue, value_z * rvalue, value_w * rvalue);
		};
		IVec4* operator/(float rvalue) {
			float inv = 1.0f / rvalue;
			return new IVec4(value_x * inv, value_y * inv, value_z * inv, value_w * inv);
		};
		float dot(IVec4* rvalue) {
			return value_x * rvalue->value_x + value_y * rvalue->value_y + value_z * rvalue->value_z + value_w * rvalue->value_w;
		};
		IVec4* cross(IVec4* rvalue) {
			return new IVec4(0, 0, 0, 0);
		};
		int value_x;
		int value_y;
		int value_z;
		int value_w;
	};
}
