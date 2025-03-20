#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Pathfinding.generated.h"

USTRUCT()
struct FPathNode
{
	GENERATED_BODY()

	FVector2D Position;
	float GCost;  // Cost from start node
	float HCost;  // Heuristic cost to target node
	float FCost() const { return GCost + HCost; }

	TSharedPtr<FPathNode> Parent;

	FPathNode() : Position(FVector2D::ZeroVector), GCost(0), HCost(0), Parent(nullptr) {}
	FPathNode(FVector2D InPosition) : Position(InPosition), GCost(0), HCost(0), Parent(nullptr) {}
};

UCLASS()
class TACTICALRPG_API APathfinding : public AActor
{
	GENERATED_BODY()
	
public:	
	APathfinding();

public:
	TArray<FVector2D> FindPath(FVector2D Start, FVector2D Goal, const TSet<FVector2D>& Obstacles, int GridSizeX, int GridSizeY);
	float CalculateHeuristic(FVector2D Node, FVector2D Target);
	TArray<FVector2D> ReconstructPath(TSharedPtr<FPathNode> Node);

};
