#pragma once
#include <cassert>
#include <fstream>
#include "Math.h"
#include "DataTypes.h"

namespace dae
{
	namespace GeometryUtils
	{
#pragma region Sphere HitTest
		//SPHERE HIT-TESTS
		inline bool HitTest_Sphere(const Sphere& sphere, const Ray& ray, HitRecord& hitRecord, bool ignoreHitRecord = false)
		{
			//todo W1
			float A = Vector3::Dot(ray.direction, ray.direction);
			float B = Vector3::Dot((2*ray.direction), ray.origin - sphere.origin);
			float C = Vector3::Dot(ray.origin - sphere.origin, (ray.origin - sphere.origin)) - (sphere.radius * sphere.radius);
			float discriminant = (B * B) - (4 * A * C);

			if (discriminant <= 0)
			{
				return false;
			}

			float t = (-B - sqrtf(discriminant)) / (2 * A);

			if (t < ray.min)
			{
				t = (-B + sqrtf(discriminant)) / (2 * A);
				if (t < ray.min)
				{
					return false;
				}
			}
			if (t > ray.max || t >= hitRecord.t)
			{
				return false;
			}

			if (ignoreHitRecord)
			{
				return true;
			}

			hitRecord.t = t;
			hitRecord.origin = ray.origin + (ray.direction * t);
			hitRecord.normal = (hitRecord.origin - sphere.origin).Normalized();
			hitRecord.materialIndex = sphere.materialIndex;
			hitRecord.didHit = true;

			return true;
		}

		inline bool HitTest_Sphere(const Sphere& sphere, const Ray& ray)
		{
			HitRecord temp{};
			return HitTest_Sphere(sphere, ray, temp, true);
		}
#pragma endregion
#pragma region Plane HitTest
		//PLANE HIT-TESTS
		inline bool HitTest_Plane(const Plane& plane, const Ray& ray, HitRecord& hitRecord, bool ignoreHitRecord = false)
		{
			//todo W1
			float t = Vector3::Dot((plane.origin - ray.origin), plane.normal) / Vector3::Dot(ray.direction, plane.normal);
			
			if (t < ray.min)
			{
				return false;
			}
			if (t > ray.max)
			{
				return false;
			}
			if (t >= hitRecord.t)
			{
				return false;
			}

			hitRecord.t = t;
			hitRecord.origin = ray.origin + (ray.direction * t);
			hitRecord.normal = plane.normal;
			hitRecord.materialIndex = plane.materialIndex;
			hitRecord.didHit = true;

			return true;
		}

		inline bool HitTest_Plane(const Plane& plane, const Ray& ray)
		{
			HitRecord temp{};
			return HitTest_Plane(plane, ray, temp, true);
		}
#pragma endregion
#pragma region Triangle HitTest
		//TRIANGLE HIT-TESTS
		inline bool HitTest_Triangle(const Triangle& triangle, const Ray& ray, HitRecord& hitRecord, bool ignoreHitRecord = false)
		{
			//todo W5
			const Vector3 v0V1 = triangle.v1 - triangle.v0;
			const Vector3 v0V2 = triangle.v2 - triangle.v0;
			const Vector3 pVec = Vector3::Cross(ray.direction, v0V2);

			const float det = Vector3::Dot(v0V1, pVec);

			switch (triangle.cullMode)
			{
			case dae::TriangleCullMode::BackFaceCulling:
				if (det < FLT_EPSILON)
					return false;
				break;
			case dae::TriangleCullMode::FrontFaceCulling:
				if (det > FLT_EPSILON)
					return false;
				break;
			case dae::TriangleCullMode::NoCulling:
				if (fabs(det) < FLT_EPSILON)
				{
					return false;
				}
				break;
			}
			
			const float invDet = 1.f / det;
			const float u = Vector3::Dot(ray.origin - triangle.v0, pVec) * invDet;
			if (u < 0 || u > 1)
			{
				return false;
			}
			const Vector3 qVec = Vector3::Cross(ray.origin - triangle.v0, v0V1);
			const float v = Vector3::Dot(ray.direction, qVec) * invDet;
			if (v < 0 || u + v >1)
			{
				return false;
			}
			const float t = Vector3::Dot(v0V2, qVec) * invDet;
			
			if ((t > ray.max) || (t < ray.min))
			{
				return false;
			}
			if (ignoreHitRecord != true)
			{
				if (t > hitRecord.t)
				{
					return false;
				}
				hitRecord.t = t;
				hitRecord.didHit = true;
				hitRecord.materialIndex = triangle.materialIndex;
				hitRecord.normal = triangle.normal;
				hitRecord.origin = ray.origin + t * ray.direction;
			}
			return true;
		}

		inline bool HitTest_Triangle(const Triangle& triangle, const Ray& ray)
		{
			HitRecord temp{};
			return HitTest_Triangle(triangle, ray, temp, true);
		}
#pragma endregion

#pragma region SlabTest
		inline bool SlapTestTriangleMesh(const TriangleMesh& mesh, const Ray& ray)
		{
			float tmin = std::min(
				(mesh.transformedMinAABB.x - ray.origin.x) / ray.direction.x,
				(mesh.transformedMaxAABB.x - ray.origin.x) / ray.direction.x);
			float tmax = std::max(
				(mesh.transformedMinAABB.x - ray.origin.x) / ray.direction.x,
				(mesh.transformedMaxAABB.x - ray.origin.x) / ray.direction.x);
			tmin = std::max(
				tmin,
				std::min(
					(mesh.transformedMinAABB.y - ray.origin.y) / ray.direction.y,
					(mesh.transformedMaxAABB.y - ray.origin.y) / ray.direction.y));
			tmax = std::min(
				tmax,
				std::max(
					(mesh.transformedMinAABB.y - ray.origin.y) / ray.direction.y,
					(mesh.transformedMaxAABB.y - ray.origin.y) / ray.direction.y));
			tmin = std::max(
				tmin,
				std::min(
					(mesh.transformedMinAABB.z - ray.origin.z) / ray.direction.z,
					(mesh.transformedMaxAABB.z - ray.origin.z) / ray.direction.z));
			tmax = std::min(
				tmax,
				std::max(
					(mesh.transformedMinAABB.z - ray.origin.z) / ray.direction.z,
					(mesh.transformedMaxAABB.z - ray.origin.z) / ray.direction.z));
			return tmax > 0 && tmax >= tmin;
		}
#pragma endregion
#pragma region TriangeMesh HitTest
		inline bool HitTest_TriangleMesh(const TriangleMesh& mesh, const Ray& ray, HitRecord& hitRecord, bool ignoreHitRecord = false)
		{
			//todo W5
			if(!SlapTestTriangleMesh(mesh, ray))
			{
				return false;
			}

			Triangle triangle{};
			triangle.cullMode = mesh.cullMode;
			triangle.materialIndex = mesh.materialIndex;

			for(int index{}; index < static_cast<int>(mesh.indices.size()); index += 3)
			{
				Vector3 v0{ mesh.transformedPositions[mesh.indices[index]] };
				Vector3 v1{ mesh.transformedPositions[mesh.indices[index+1]] };
				Vector3 v2{ mesh.transformedPositions[mesh.indices[index+2]] };

				Vector3 normal{ mesh.transformedNormals[index/3] };

				triangle.v0 = v0; 
				triangle.v1 = v1; 
				triangle.v2 = v2;

				triangle.normal = normal; 

				if(HitTest_Triangle(triangle, ray, hitRecord, ignoreHitRecord))
				{
					if(ignoreHitRecord)
					{
						return true;
					}
				}
			}
			return hitRecord.didHit;
		}

		inline bool HitTest_TriangleMesh(const TriangleMesh& mesh, const Ray& ray)
		{
			HitRecord temp{};
			return HitTest_TriangleMesh(mesh, ray, temp, true);
		}
#pragma endregion
	}

	namespace LightUtils
	{
		//Direction from target to light
		inline Vector3 GetDirectionToLight(const Light& light, const Vector3 origin)
		{
			//todo W3
			return {light.origin - origin};
		}

		inline ColorRGB GetRadiance(const Light& light, const Vector3& target)
		{
			//todo W3

			if(light.type == LightType::Directional)
			{
				return light.color * light.intensity;
			}

			return light.color * (light.intensity / (light.origin - target).SqrMagnitude());
		}
	}

	namespace Utils
	{
		//Just parses vertices and indices
#pragma warning(push)
#pragma warning(disable : 4505) //Warning unreferenced local function
		static bool ParseOBJ(const std::string& filename, std::vector<Vector3>& positions, std::vector<Vector3>& normals, std::vector<int>& indices)
		{
			std::ifstream file(filename);
			if (!file)
				return false;

			std::string sCommand;
			// start a while iteration ending when the end of file is reached (ios::eof)
			while (!file.eof())
			{
				//read the first word of the string, use the >> operator (istream::operator>>) 
				file >> sCommand;
				//use conditional statements to process the different commands	
				if (sCommand == "#")
				{
					// Ignore Comment
				}
				else if (sCommand == "v")
				{
					//Vertex
					float x, y, z;
					file >> x >> y >> z;
					positions.push_back({ x, y, z });
				}
				else if (sCommand == "f")
				{
					float i0, i1, i2;
					file >> i0 >> i1 >> i2;

					indices.push_back((int)i0 - 1);
					indices.push_back((int)i1 - 1);
					indices.push_back((int)i2 - 1);
				}
				//read till end of line and ignore all remaining chars
				file.ignore(1000, '\n');

				if (file.eof()) 
					break;
			}

			//Precompute normals
			for (uint64_t index = 0; index < indices.size(); index += 3)
			{
				uint32_t i0 = indices[index];
				uint32_t i1 = indices[index + 1];
				uint32_t i2 = indices[index + 2];

				Vector3 edgeV0V1 = positions[i1] - positions[i0];
				Vector3 edgeV0V2 = positions[i2] - positions[i0];
				Vector3 normal = Vector3::Cross(edgeV0V1, edgeV0V2);

				if(isnan(normal.x))
				{
					int k = 0;
				}

				normal.Normalize();
				if (isnan(normal.x))
				{
					int k = 0;
				}

				normals.push_back(normal);
			}

			return true;
		}
#pragma warning(pop)
	}
}