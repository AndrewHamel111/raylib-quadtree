#ifndef QUAD_TREE_H_
#define QUAD_TREE_H_

#include "raylib.h"

#define QUAD_TREE_CAPACITY 4

typedef struct QuadTreeQueryResult
{

	Vector2* array;
	int count;
	int capacity;

} QuadTreeQueryResult;

typedef struct QuadTree
{

	Rectangle boundary;

	struct QuadTree* nw;
	struct QuadTree* ne;
	struct QuadTree* sw;
	struct QuadTree* se;

	Vector2 points[QUAD_TREE_CAPACITY];
	int count;
	bool subdivided;

} QuadTree;

QuadTree QuadTreeCreate(float x, float y, float w, float h);
bool QuadTreeSubdivide(QuadTree* tree);
bool QuadTreeInsert(QuadTree* tree, Vector2 point);

bool QuadTreeQueryAddPoint(QuadTreeQueryResult* result, Vector2 point);
bool QuadTreeQuery(QuadTree tree, Rectangle query, QuadTreeQueryResult* result);

void QuadTreeQueryDebugDraw(QuadTreeQueryResult result, Rectangle query);
void QuadTreeDebugDraw(QuadTree tree);

#endif