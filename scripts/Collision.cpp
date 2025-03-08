#include "Collision.h"
#include "PhysicsMath.h"
#include <iostream>

namespace Collision {


	HitResult IntersectCirclePolygon(sf::Vector2f circleCenter, float circleRadius,
		const std::vector<sf::Vector2f>& vertices) 	
	{
		HitResult result;
		// try to find lowest depth
		result.depth = INFINITY;
		result.normal = { 0.f,0.f };

		//std::cout << "A size:" << verticesA.size() << "\tB size:" << verticesB.size() << '\n';

		for (unsigned int i{ 0 }; i < vertices.size(); i++) {

			sf::Vector2f va = vertices[i];
			sf::Vector2f vb = vertices[(i + 1) % vertices.size()];

			sf::Vector2f edge = vb - va;
			sf::Vector2f axis = { -edge.y, edge.x };
			axis = axis.normalized();

			MinMax resultA = ProjectVertices(vertices, axis);
			MinMax resultB = ProjectCircle(circleCenter,circleRadius, axis);

			// check for overlap
			if (resultA.min >= resultB.max || resultB.min >= resultA.max)
				return result;


			float depth = std::min(std::abs(resultB.max - resultA.min), std::abs(resultA.max - resultB.max));
			if (depth < result.depth) {
				result.depth = depth;
				result.normal = axis;
			}
		}

		int cpIndex = FindClosestPointOnPolygon(circleCenter, vertices);
		sf::Vector2f cp = vertices[cpIndex];

		sf::Vector2f axis = cp - circleCenter;

		MinMax resultA = ProjectVertices(vertices, axis);
		MinMax resultB = ProjectCircle(circleCenter, circleRadius, axis);

		// check for overlap
		if (resultA.min >= resultB.max || resultB.min >= resultA.max)
			return result;

		result.collided = true;


		float depth = std::min(std::abs(resultB.max - resultA.min), std::abs(resultA.max - resultB.max));
		if (depth < result.depth) {
			result.depth = depth;
			result.normal = axis;
		}


		sf::Vector2f polygonCenter = FindArithmeticMean(vertices);

		sf::Vector2f direction = polygonCenter - circleCenter;
		std::cout << "Direction:" << direction.x << "," << direction.y << '\n';

		if (direction.dot(result.normal) < 0.f)
			result.normal = -result.normal;

		return result;
	}

	HitResult IntersectPolygons(const std::vector<sf::Vector2f>& verticesA, 
		const std::vector<sf::Vector2f>& verticesB)
	{
		HitResult result;
		// try to find lowest depth
		result.depth = INFINITY;
		result.normal = { 0.f,0.f };

		//std::cout << "A size:" << verticesA.size() << "\tB size:" << verticesB.size() << '\n';

		for (unsigned int i{ 0 }; i < verticesA.size(); i++) {

			sf::Vector2f va = verticesA[i];
			sf::Vector2f vb = verticesA[(i + 1) % verticesA.size()];

			sf::Vector2f edge = vb - va;
			sf::Vector2f axis = { -edge.y, edge.x };

			MinMax resultA = ProjectVertices(verticesA, axis);
			MinMax resultB = ProjectVertices(verticesB, axis);

			// check for overlap
			if (resultA.min >= resultB.max || resultB.min >= resultA.max)
				return result;


			float depth = std::min(std::abs( resultB.max - resultA.min), std::abs(resultA.max - resultB.max));
			if (depth < result.depth) {
				result.depth = depth;
				result.normal = axis;
			}
		}
		for (unsigned int i{ 0 }; i < verticesB.size(); i++) {

			sf::Vector2f va = verticesB[i];
			sf::Vector2f vb = verticesB[(i + 1) % verticesB.size()];
			/*
			sf::Vector2f edge = vb - va;
			//edge.perpendicular() would also suffice this makes it more obvious
			sf::Vector2f axis = { -edge.y,edge.x };
			axis = axis.normalized();
			*/

			sf::Vector2f edge = vb - va;
			sf::Vector2f axis = { -edge.y, edge.x };

			MinMax resultA = ProjectVertices(verticesA, axis);
			MinMax resultB = ProjectVertices(verticesB, axis);

			// check for overlap
			if (resultA.min >= resultB.max || resultB.min >= resultA.max)
				return result;

			float depth = std::min(std::abs(resultB.max - resultA.min), std::abs(resultA.max - resultB.max));
			//float depth = std::min(resultB.max - resultA.min, resultA.max - resultB.max);
			if (depth < result.depth) {
				result.depth = depth;
				result.normal = axis;
			}
		}
		//std::cout << " WE COLLIDED\n";
		result.collided = true;
		std::cout << "AxisDepth:" << result.depth << '\n';

		//result.depth  = result.depth / result.normal.length();
		//result.normal = result.normal.normalized();
		


		sf::Vector2f centerA = FindArithmeticMean(verticesA);
		sf::Vector2f centerB = FindArithmeticMean(verticesB);
		
		//std::cout << "A-CENTER: " << centerA.x << "," << centerA.y << '\n';
		//std::cout << "B-CENTER: " << centerB.x << "," << centerB.y << '\n';

		sf::Vector2f direction = centerB - centerA;
		std::cout << "Direction:" << direction.x << "," << direction.y << '\n';

		if (direction.dot(result.normal) < 0.f)
			result.normal = -result.normal;


		return result;
	}

	int FindClosestPointOnPolygon(sf::Vector2f circleCenter, const std::vector<sf::Vector2f>& vertices)
	{
		int result = -1;
		float minDistance = INFINITY;

		for (unsigned int i{ 0 }; i < vertices.size(); i++) {
			sf::Vector2f v = vertices[i];
			float distance = PhysicsMath::Distance(v, circleCenter);

			if (distance < minDistance) {
				minDistance = distance;
				result = i;
			}
		}
		return result;
	}

	sf::Vector2f FindArithmeticMean(const std::vector<sf::Vector2f>& vertices)
	{
		float sumX = 0.f;
		float sumY = 0.f;

		for (int i = 0; i < vertices.size(); i++) {
			sumX += vertices[i].x;
			sumY += vertices[i].y;
		}
		return { sumX / vertices.size(), sumY / vertices.size() };
	}

	MinMax ProjectVertices(const std::vector<sf::Vector2f>& vertices, sf::Vector2f axis)
	{
		MinMax result{};

		for (unsigned int i{ 0 }; i < vertices.size(); i++) {
			sf::Vector2f v = vertices[i];
			float proj = axis.dot(v);

			if (proj < result.min)
				result.min = proj;
			if (proj > result.max)
				result.max = proj;
		}
		return result;
	}

	MinMax ProjectCircle(sf::Vector2f center, float radius, sf::Vector2f axis)
	{
		MinMax result{};


		sf::Vector2f direction = axis.normalized();
		sf::Vector2f directionAndRadius = direction * radius;

		sf::Vector2f point1 = center + directionAndRadius;
		sf::Vector2f point2 = center - directionAndRadius;

		result.min = point1.dot(axis);
		result.max = point2.dot(axis);

		if (result.min > result.max) {
			std::swap(result.min, result.max);
		}
		return result;
	}



	HitResult IntersectCircles(sf::Vector2f centerA, float radiusA, sf::Vector2f centerB, float radiusB)
	{
		HitResult result;
		float distance = PhysicsMath::Distance(centerA, centerB);
		float radii = radiusA + radiusB;

		if (distance >= radii)
			return result;

		result.collided = true;
		result.normal = sf::Vector2f(centerB - centerA).normalized();
		result.depth = radii - distance;
		return result;
	}


}
