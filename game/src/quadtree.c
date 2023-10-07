#include "quadtree.h"
#include "raymath.h"

#include <stdlib.h>

#define QUAD_TREE_QUERY_INITIAL_CAPACITY 32

QuadTree QuadTreeCreate(float x, float y, float w, float h)
{
	QuadTree tree = { 0 };

	tree.boundary = (Rectangle){ x, y, w, h };

	tree.nw = NULL;
	tree.ne = NULL;
	tree.sw = NULL;
	tree.se = NULL;

	tree.count = 0;
	tree.subdivided = false;

	return tree;
}

bool QuadTreeSubdivide(QuadTree* tree)
{
	if (tree->subdivided)
	{
		TraceLog(LOG_ERROR, "QT: Tried to subdivide QuadTree but it has already been subdivided!");
		return false;
	}

	QuadTree* mem = (QuadTree*)calloc(4, sizeof(QuadTree));
	if (mem == NULL)
	{
		TraceLog(LOG_FATAL, "QT: Failed to allocate memory for QuadTreeSubdivide!");
		return false;
	}

	tree->nw = mem;
	tree->ne = mem + 1;
	tree->sw = mem + 2;
	tree->se = mem + 3;

	float x = tree->boundary.x;
	float y = tree->boundary.y;
	float w = tree->boundary.width;
	float h = tree->boundary.height;

	*(tree->nw) = QuadTreeCreate(x, y, w/2, h/2);
	*(tree->ne) = QuadTreeCreate(x + w/2, y, w / 2, h / 2);
	*(tree->sw) = QuadTreeCreate(x, y + h/2, w / 2, h / 2);
	*(tree->se) = QuadTreeCreate(x + w/2, y + h/2, w / 2, h / 2);

	tree->subdivided = true;

	return true;
}

bool QuadTreeInsert(QuadTree* tree, Vector2 point)
{
	if (!CheckCollisionPointRec(point, tree->boundary))
	{
		return;
	}

	if (tree->count == QUAD_TREE_CAPACITY)
	{
		if (!tree->subdivided && !QuadTreeSubdivide(tree))
		{
			return false;
		}

		return QuadTreeInsert(tree->nw, point)
			|| QuadTreeInsert(tree->ne, point)
			|| QuadTreeInsert(tree->sw, point)
			|| QuadTreeInsert(tree->se, point);
	}

	tree->points[tree->count] = point;
	tree->count++;
	return true;
}

bool QuadTreeQueryAddPoint(QuadTreeQueryResult* result, Vector2 point)
{
	if (result->array == NULL)
	{
		result->array = (Vector2*)calloc(QUAD_TREE_QUERY_INITIAL_CAPACITY, sizeof(Vector2));
		result->capacity = QUAD_TREE_QUERY_INITIAL_CAPACITY;
		result->count = 0;
	}

	if (result->count == result->capacity)
	{
		int newCapacity = result->capacity * 2;
		Vector2* mem = (Vector2*)realloc(result->array, newCapacity * sizeof(Vector2));
		if (mem == NULL)
		{
			TraceLog(LOG_FATAL, "Could not realloc for QuadTreeQueryAddPoint!");
			return false;
		}

		result->array = mem;
		result->capacity = newCapacity;
	}

	result->array[result->count] = point;
	result->count++;
	return true;
}

bool QuadTreeQuery(QuadTree tree, Rectangle query, QuadTreeQueryResult* result)
{
	if (!CheckCollisionRecs(tree.boundary, query))
	{
		return;
	}

	for (int i = 0; i < tree.count; i++)
	{
		if (!CheckCollisionPointRec(tree.points[i], query))
		{
			continue;
		}

		if (!QuadTreeQueryAddPoint(result, tree.points[i]))
		{
			return false;
		}
	}

	if (!tree.subdivided)
	{
		return;
	}

	QuadTreeQuery(*tree.nw, query, result);
	QuadTreeQuery(*tree.ne, query, result);
	QuadTreeQuery(*tree.sw, query, result);
	QuadTreeQuery(*tree.se, query, result);
}

void QuadTreeQueryDebugDraw(QuadTreeQueryResult result, Rectangle query)
{
	for (int i = 0; i < result.count; i++)
	{
		DrawCircleV(result.array[i], 5, GREEN);
	}

	DrawRectangleLinesEx(query, 2, GREEN);
}

void QuadTreeDebugDraw(QuadTree tree)
{
	if (tree.subdivided)
	{
		QuadTreeDebugDraw(*tree.nw);
		QuadTreeDebugDraw(*tree.ne);
		QuadTreeDebugDraw(*tree.sw);
		QuadTreeDebugDraw(*tree.se);
	}

	for (int i = 0; i < tree.count; i++)
	{
		DrawCircleV(tree.points[i], 3, WHITE);
	}

	DrawRectangleLinesEx(tree.boundary, 1, DARKGRAY);
}