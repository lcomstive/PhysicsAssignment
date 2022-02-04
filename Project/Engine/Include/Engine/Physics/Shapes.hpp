#pragma once
#include <vector>
#include <glm/glm.hpp>

namespace Engine::Physics
{
	struct Ray;
	struct Line;
	struct Plane;

	glm::vec3 project(glm::vec3& length, glm::vec3& direction);

	struct Interval
	{
		float Min;
		float Max;
	};

	struct Line
	{
		glm::vec3 Start;
		glm::vec3 End;

		Line(glm::vec3 start, glm::vec3 end);

		float Length();
		float LengthSqr();

		bool IsPointOn(glm::vec3& point);
		glm::vec3 GetClosestPoint(glm::vec3& point);
	};

	struct RaycastHit
	{
		glm::vec3 Point	 = { 0, 0, 0 };
		glm::vec3 Normal = { 0, 0, 1 };
		float Distance	 = -1.0f;
		bool Hit		 = false;
	};

	struct Ray
	{
		glm::vec3 Origin = { 0, 0, 0 };
		glm::vec3 Direction = { 0, 0, 1 };

		bool IsPointOn(glm::vec3& point);
		glm::vec3 GetClosestPoint(glm::vec3& point);

		static Ray FromLine(Line& line);
		static Ray FromPoints(glm::vec3 start, glm::vec3 end);
	};

	/// <summary>
	/// Axis-Aligned Bounding Box
	/// </summary>
	struct AABB
	{
		glm::vec3 Position = { 0, 0, 0 };
		glm::vec3 Extents = { 0.5f, 0.5f, 0.5f };

		glm::vec3 Min();
		glm::vec3 Max();

		static AABB FromMinMax(glm::vec3 min, glm::vec3 max);

		bool Intersects(AABB& other);
		bool IsPointInside(glm::vec3& point);
		Interval GetInterval(const glm::vec3& axis);
		glm::vec3 GetClosestPoint(glm::vec3& point);

		/// <returns>Success state of raycast</returns>
		bool Raycast(Ray& ray, RaycastHit* outResult = nullptr);

		/// <returns>Success if line intersects this object</returns>
		bool LineTest(Line& line);
	};

	/// <summary>
	/// Oriented Bounding Box
	/// </summary>
	struct OBB
	{
		glm::vec3 Position;
		glm::vec3 Extents; // Half size
		glm::mat3 Orientation;

		bool IsPointInside(glm::vec3& point);
		glm::vec3 GetClosestPoint(glm::vec3& point);
		Interval GetInterval(const glm::vec3& axis);
		bool OverlapOnAxis(OBB& other, glm::vec3& axis);

		std::vector<Line> GetEdges();
		std::vector<Plane> GetPlanes();
		std::vector<glm::vec3> GetVertices();
		
		std::vector<glm::vec3> ClipEdges(std::vector<Line> edges);
		bool ClipToPlane(Plane& plane, Line& line, glm::vec3* result);
		float PenetrationDepth(OBB& other, glm::vec3& axis, bool* outShouldFlip);

		/// <returns>Success state of raycast</returns>
		bool Raycast(Ray& ray, RaycastHit* outResult = nullptr);

		/// <returns>Success if line intersects this object</returns>
		bool LineTest(Line& line);
	};

	struct Sphere
	{
		glm::vec3 Position = { 0, 0, 0 };
		float Radius = 1.0f;

		bool IsPointInside(glm::vec3& point);

		/// <summary>
		/// Gets the closest point along the sphere's surface
		/// </summary>
		glm::vec3 GetClosestPoint(glm::vec3& point);

		/// <returns>Success state of raycast</returns>
		bool Raycast(Ray& ray, RaycastHit* outResult = nullptr);

		/// <returns>Success if line intersects this object</returns>
		bool LineTest(Line& line);
	};

	enum class PlaneIntersection { None, Behind, InFront, Intersecting };

	struct Triangle
	{
		union
		{
			struct { glm::vec3 A, B, C; };
			glm::vec3 Points[3];
			float Values[9];
		};

		Triangle();
		Triangle(glm::vec3 a, glm::vec3 b, glm::vec3 c);

		bool IsPointInside(glm::vec3& point);
		glm::vec3 GetClosestPoint(glm::vec3& point);

		bool Intersects(OBB& obb);
		bool Intersects(AABB& aabb);
		bool Intersects(Plane& plane);
		bool Intersects(Sphere& sphere);
		bool Intersects(Triangle& triangle);

		Interval GetInterval(glm::vec3& axis);
		bool OverlapOnAxis(OBB& aabb, glm::vec3& axis);
		bool OverlapOnAxis(AABB& aabb, glm::vec3& axis);
		bool OverlapOnAxis(Triangle& tri, glm::vec3& axis);

		glm::vec3 Barycentric(glm::vec3& point);

		/// <returns>Success state of raycast</returns>
		bool Raycast(Ray& ray, RaycastHit* outResult = nullptr);

		/// <returns>Success if line intersects this object</returns>
		bool LineTest(Line& line);

	private:
		glm::vec3 SatCrossEdge(glm::vec3& a, glm::vec3& b, glm::vec3& c, glm::vec3& d);
	};

	struct Plane
	{
		/// <summary>
		/// Distance along normal from world origin
		/// </summary>
		float Distance = 0.0f;

		/// <summary>
		/// Upwards direction of plane
		/// </summary>
		glm::vec3 Normal = { 0, 1, 0 };

		Plane(glm::vec3 normal = { 0, 1, 0 }, float distance = 0.0f);
		Plane(Triangle& tri);

		/// <summary>
		/// Checks where point is relative to plane
		/// </summary>
		PlaneIntersection CheckPoint(glm::vec3 point);

		glm::vec3 GetClosestPoint(glm::vec3& point);

		/// <returns>Success state of raycast</returns>
		bool Raycast(Ray& ray, RaycastHit* outResult = nullptr);

		/// <returns>Success if line intersects this object</returns>
		bool LineTest(Line& line);

		float PlaneEquation(glm::vec3 point);
	};
}