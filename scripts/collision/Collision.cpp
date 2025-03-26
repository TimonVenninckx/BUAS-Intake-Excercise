#include "Collision.h"
#include "../PhysicsMath.h"
#include <iostream>



namespace Collision {


	struct PointSegment {
		float distanceSquared{};
		sf::Vector2f cp{};
	};

	sf::Vector2f findCirclesContactPoint(sf::Vector2f centerA, float radiusA, sf::Vector2f centerB);
	sf::Vector2f findCirclePolygonContactPoint(sf::Vector2f circleCenter, float circleRadius, sf::Vector2f polygonCenter, const std::vector<sf::Vector2f>& polygonVertices);
	ContactResult findPolygonsContactPoint(const std::vector<sf::Vector2f>& verticesA,const std::vector<sf::Vector2f>& verticesB);


	PointSegment pointSegmentDistance(sf::Vector2f p, sf::Vector2f a, sf::Vector2f b) {
		PointSegment result;
		
		sf::Vector2f ab = b - a;
		sf::Vector2f ap = p - a;

		float proj = ap.dot(ab);
		float abLenSq = ab.x * ab.x + ab.y * ab.y;
		float d = proj / abLenSq;

		if (d <= 0.f) {
			result.cp = a;
		}
		else if (d >= 1.f) {
			result.cp = b;
		}
		else {
			result.cp = a + ab * d;
		}

		result.distanceSquared = PhysicsMath::distanceSquared(p, result.cp);
		
		return result;
	}

	bool IntersectAABB(AABB& a, AABB& b)
	{
		if(a.max.x <= b.min.x || b.max.x <= a.min.x ||
			a.max.y <= b.min.y || b.max.y <= a.min.y)
			return false;
		return true;
	}

	ContactResult findContactPoints(PhysicsBody* bodyA, PhysicsBody* bodyB) {
		ContactResult result;
		if (bodyA->getShapeType() == ShapeType::Box) {
			if (bodyB->getShapeType() == ShapeType::Box) {
				result = findPolygonsContactPoint(bodyA->getTransformedVertices(), bodyB->getTransformedVertices());
			}
			else if (bodyB->getShapeType() == ShapeType::Triangle) {
				result = findPolygonsContactPoint(bodyA->getTransformedVertices(), bodyB->getTransformedVertices());
			}
			else if (bodyB->getShapeType() == ShapeType::Circle) {
				result.contact1 = findCirclePolygonContactPoint(bodyB->getPosition(),bodyB->getRadius(),bodyA->getPosition(),bodyA->getTransformedVertices());
				result.contactCount = 1;
			}
		}
		else if (bodyA->getShapeType() == ShapeType::Circle) {
			if (bodyB->getShapeType() == ShapeType::Box) {
				result.contact1 = findCirclePolygonContactPoint(bodyA->getPosition(), bodyA->getRadius(), bodyB->getPosition(), bodyB->getTransformedVertices());
				result.contactCount = 1;
			}
			else if (bodyB->getShapeType() == ShapeType::Triangle) {
				result = findPolygonsContactPoint(bodyA->getTransformedVertices(), bodyB->getTransformedVertices());
			}
			else if (bodyB->getShapeType() == ShapeType::Circle) {
				result.contact1 = findCirclesContactPoint(bodyA->getPosition(), bodyA->getRadius(), bodyB->getPosition());
				result.contactCount = 1;
			}
		}
		if (bodyA->getShapeType() == ShapeType::Triangle) {
			if (bodyB->getShapeType() == ShapeType::Box) {
				result = findPolygonsContactPoint(bodyA->getTransformedVertices(), bodyB->getTransformedVertices());
			}
			else if (bodyB->getShapeType() == ShapeType::Triangle) {
				result = findPolygonsContactPoint(bodyA->getTransformedVertices(), bodyB->getTransformedVertices());
			}
			else if (bodyB->getShapeType() == ShapeType::Circle) {
				result.contact1 = findCirclePolygonContactPoint(bodyB->getPosition(), bodyB->getRadius(), bodyA->getPosition(), bodyA->getTransformedVertices());
				result.contactCount = 1;
			}
		}
		return result;
	}

	sf::Vector2f findCirclesContactPoint(sf::Vector2f centerA, float radiusA, sf::Vector2f centerB)
	{
		sf::Vector2f ab = centerB - centerA;
		if(ab != zeroVector)
			ab = ab.normalized();
		
		return centerA + ab * radiusA;
	}

	sf::Vector2f findCirclePolygonContactPoint(sf::Vector2f circleCenter, float circleRadius, sf::Vector2f polygonCenter, const std::vector<sf::Vector2f>& polygonVertices) {

		sf::Vector2f cp = zeroVector;
		float minDistSq = INFINITY;

		for (unsigned int i{ 0 }; i < polygonVertices.size(); i++) {
			sf::Vector2f va = polygonVertices[i];
			sf::Vector2f vb = polygonVertices[(i + 1) % polygonVertices.size()];

			PointSegment s = Collision::pointSegmentDistance(circleCenter, va, vb);

			if (s.distanceSquared < minDistSq) {
				minDistSq = s.distanceSquared;
				cp = s.cp;
			}
		}
		return cp;
	}

	ContactResult findPolygonsContactPoint(const std::vector<sf::Vector2f>& verticesA, const std::vector<sf::Vector2f>& verticesB) {

		ContactResult result;

		PointSegment cp{ INFINITY,zeroVector};
		
		for (unsigned int i{ 0 }; i < verticesA.size(); i++) {
			sf::Vector2f p = verticesA[i];

			for (unsigned int j{ 0 }; j < verticesB.size(); j++) {
				sf::Vector2f va = verticesB[j];
				sf::Vector2f vb = verticesB[(j+ 1) %verticesB.size()];
				PointSegment point = pointSegmentDistance(p, va, vb);

				if (PhysicsMath::nearlyEqual(cp.distanceSquared , point.distanceSquared)) {
					//std::cout << "point: " << cp.cp.x << ',' << cp.cp.y << " and point:" << point.cp.x << "," << point.cp.y << '\n';
					if (!PhysicsMath::nearlyEqual(point.cp, result.contact1)) {
						result.contact2 = point.cp;
						result.contactCount = 2;
					}
				}
				else if (point.distanceSquared < cp.distanceSquared) {
					cp = point;
					result.contact1 = cp.cp;
					result.contactCount = 1;
				}
			}
		}

		for (unsigned int i{ 0 }; i < verticesB.size(); i++) {
			sf::Vector2f p = verticesB[i];

			for (unsigned int j{ 0 }; j < verticesA.size(); j++) {
				sf::Vector2f va = verticesA[j];
				sf::Vector2f vb = verticesA[(j + 1) % verticesA.size()];
				PointSegment point = pointSegmentDistance(p, va, vb);

				if (PhysicsMath::nearlyEqual(cp.distanceSquared, point.distanceSquared)) {

					if (!PhysicsMath::nearlyEqual(point.cp, result.contact1)) {
						result.contact2 = point.cp;
						result.contactCount = 2;
					}
				}
				else if (point.distanceSquared < cp.distanceSquared) {
					cp = point;
					result.contact1 = cp.cp;
					result.contactCount = 1;
				}
			}
		}

		return result;
	}



	HitResult collide(PhysicsBody* bodyA, PhysicsBody* bodyB) {
		HitResult result{};

		if (bodyA->getShapeType() == ShapeType::Box) {
			if (bodyB->getShapeType() == ShapeType::Box) {
				return Collision::intersectPolygons(bodyA->getPosition(), bodyA->getTransformedVertices(), bodyB->getPosition(), bodyB->getTransformedVertices());
			}
			else if (bodyB->getShapeType() == ShapeType::Triangle) {
				return Collision::intersectPolygons(bodyA->getPosition(), bodyA->getTransformedVertices(), bodyB->getPosition(), bodyB->getTransformedVertices());
			}
			else if (bodyB->getShapeType() == ShapeType::Circle) {
				result = Collision::intersectCirclePolygon(bodyB->getPosition(), bodyB->getRadius(), bodyA->getPosition(), bodyA->getTransformedVertices());

				result.normal = -result.normal;
				return result;
			}
		}
		else if (bodyA->getShapeType() == ShapeType::Circle) {
			if (bodyB->getShapeType() == ShapeType::Box) {
				return Collision::intersectCirclePolygon(bodyA->getPosition(), bodyA->getRadius(), bodyB->getPosition(), bodyB->getTransformedVertices());
			}
			if (bodyB->getShapeType() == ShapeType::Triangle) {
				return Collision::intersectCirclePolygon(bodyA->getPosition(), bodyA->getRadius(), bodyB->getPosition(), bodyB->getTransformedVertices());
			}
			else if (bodyB->getShapeType() == ShapeType::Circle) {
				return Collision::intersectCircles(bodyA->getPosition(), bodyA->getRadius(), bodyB->getPosition(), bodyB->getRadius());
			}
		}
		else if (bodyA->getShapeType() == ShapeType::Triangle) {
			if (bodyB->getShapeType() == ShapeType::Box) {
				return Collision::intersectPolygons(bodyA->getPosition(), bodyA->getTransformedVertices(), bodyB->getPosition(), bodyB->getTransformedVertices());
			}
			else if (bodyB->getShapeType() == ShapeType::Triangle) {
				return Collision::intersectPolygons(bodyA->getPosition(), bodyA->getTransformedVertices(), bodyB->getPosition(), bodyB->getTransformedVertices());
			}
			else if (bodyB->getShapeType() == ShapeType::Circle) {
				result = Collision::intersectCirclePolygon(bodyB->getPosition(), bodyB->getRadius(), bodyA->getPosition(), bodyA->getTransformedVertices());

				result.normal = -result.normal;
				return result;
			}
		}
		return result;
	}

	HitResult intersectCirclePolygon(sf::Vector2f circleCenter, float circleRadius, sf::Vector2f polygonCenter, const std::vector<sf::Vector2f>& vertices)
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
			if (axis != zeroVector)
				axis = axis.normalized();

			MinMax resultA = projectVertices(vertices, axis);
			MinMax resultB = projectCircle(circleCenter, circleRadius, axis);

			// check for overlap
			if (resultA.min >= resultB.max || resultB.min >= resultA.max)
				return result;


			float depth = std::min(resultB.max - resultA.min, resultA.max - resultB.min);
			if (depth < result.depth) {
				result.depth = depth;
				result.normal = axis;
			}
		}

		int cpIndex = findClosestPointOnPolygon(circleCenter, vertices);
		sf::Vector2f cp = vertices[cpIndex];

		sf::Vector2f axis = cp - circleCenter;
		if (axis != zeroVector)
			axis = axis.normalized();

		MinMax resultA = projectVertices(vertices, axis);
		MinMax resultB = projectCircle(circleCenter, circleRadius, axis);

		// check for overlap
		if (resultA.min >= resultB.max || resultB.min >= resultA.max)
			return result;

		result.collided = true;


		float depth = std::min(std::abs(resultB.max - resultA.min), std::abs(resultA.max - resultB.max));
		if (depth < result.depth) {
			result.depth = depth;
			result.normal = axis;
		}


		sf::Vector2f direction = polygonCenter - circleCenter;
		//std::cout << "Direction:" << direction.x << "," << direction.y << '\n';

		if (direction.dot(result.normal) < 0.f)
			result.normal = -result.normal;

		return result;
	}


	HitResult intersectPolygons(sf::Vector2f centerA, const std::vector<sf::Vector2f>& verticesA, sf::Vector2f centerB, const std::vector<sf::Vector2f>& verticesB)
	{
		HitResult result;
		// try to find lowest depth
		result.depth = INFINITY;
		result.normal = { 0.f,0.f };
		


		for (unsigned int i{ 0 }; i < verticesA.size(); i++) {

			sf::Vector2f va = verticesA[i];
			sf::Vector2f vb = verticesA[(i + 1) % verticesA.size()];

			sf::Vector2f edge = vb - va;
			sf::Vector2f axis = { -edge.y, edge.x };
			
			if (axis != zeroVector)
				axis = axis.normalized();

			
			MinMax resultA = projectVertices(verticesA, axis);
			MinMax resultB = projectVertices(verticesB, axis);

			// check for overlap
			if (resultA.min >= resultB.max || resultB.min >= resultA.max)
				return result;


			float depth = std::min(resultB.max - resultA.min, resultA.max - resultB.min);
			if (depth < result.depth) {
				result.depth = depth;
				result.normal = axis;
				if (result.normal != sf::Vector2f(0.f, 1.f))
				{
					if (result.normal.y == 1.f) {
						result.normal.x = 0.f;
					}
				}
			}
		}
		for (unsigned int i{ 0 }; i < verticesB.size(); i++) {

			sf::Vector2f va = verticesB[i];
			sf::Vector2f vb = verticesB[(i + 1) % verticesB.size()];

			sf::Vector2f edge = vb - va;
			sf::Vector2f axis = { -edge.y, edge.x };
			if (axis != zeroVector)
				axis = axis.normalized();

			MinMax resultA = projectVertices(verticesA, axis);
			MinMax resultB = projectVertices(verticesB, axis);

			// check for overlap
			if (resultA.min >= resultB.max || resultB.min >= resultA.max)
				return result;

			float depth = std::min(resultB.max - resultA.min, resultA.max - resultB.min);
			if (depth < result.depth) {
				result.depth = depth;
				result.normal = axis;
				if (result.normal != sf::Vector2f(0.f, 1.f))
				{
					if (result.normal.y == 1.f) {
						result.normal.x = 0.f;
					}
				}
			}

		}

		result.collided = true;

		sf::Vector2f direction = centerB - centerA;

		if (direction.dot(result.normal) < 0.f)
			result.normal = -result.normal;

		


		return result;
	}

	int findClosestPointOnPolygon(sf::Vector2f circleCenter, const std::vector<sf::Vector2f>& vertices)
	{
		int result = -1;
		float minDistance = INFINITY;

		for (unsigned int i{ 0 }; i < vertices.size(); i++) {
			sf::Vector2f v = vertices[i];
			float distance = PhysicsMath::distance(v, circleCenter);

			if (distance < minDistance) {
				minDistance = distance;
				result = i;
			}
		}
		return result;
	}


	

	MinMax projectVertices(const std::vector<sf::Vector2f>& vertices, sf::Vector2f axis)
	{
		MinMax result{};
		result.min = INFINITY;
		result.max = -INFINITY;

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

	MinMax projectCircle(sf::Vector2f center, float radius, sf::Vector2f axis)
	{
		MinMax result{};

		sf::Vector2f direction;
		if (axis != zeroVector) // can't normalize a {0.f,0.f} vector
			direction = axis.normalized();
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



	HitResult intersectCircles(sf::Vector2f centerA, float radiusA, sf::Vector2f centerB, float radiusB)
	{
		HitResult result;
		float distance = PhysicsMath::distance(centerA, centerB);
		float radii = radiusA + radiusB;

		if (distance >= radii)
			return result;

		result.collided = true;
		result.normal = sf::Vector2f(centerB - centerA);
		// .normalized() on a 0,0 vector gives an exception
		if (result.normal != zeroVector)
			result.normal = result.normal.normalized();

		result.depth = radii - distance;
		return result;
	}


}
